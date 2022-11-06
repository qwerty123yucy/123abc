#include <thread/tcb.h>
#include <mm/mman.h>
#include <stdbool.h>



/* 
 * stack_size 0: DEFAULT_THREAD_STACK_SIZE; n: n kB
 * this function will let thread be in 'inactive' state
 * inited tcb are allocated with its stack,
 * and are able to add into g_tasks
 */
int tcb_init(struct tcb *tcb, 
		uint8_t budget, uint8_t period, 
		uint8_t stack_blocks, uint8_t priority,
		uint32_t entry){
	// since this is a new task, its register context are reset
	for(uint32_t i = 0;i < USER_REGISTER_NUM;i++){
		tcb->registers[i] = 0;
	}

	tcb->state = inactive;
	tcb->is_queued = false;	
	if(priority > MIN_PRIORITY){
		tcb->priority = MIN_PRIORITY;
	}
	else{
		tcb->priority = priority;
	}
	// by calling 'tcb_resume', pc will jump to 'entry' to run this task
	tcb->registers[pc] = entry;
	
	
	if(stack_blocks == 0 || stack_blocks > MAX_TSTACK_BLOCKS){
		tcb->stack_size = DEFAULT_TSTACK_SIZE;
	}
	else{
		tcb->stack_size = stack_blocks * TSTACK_BLOCK;
	}
	/* 
	 * tasks' stack are allocated in heap with 4 bytes align
	 * (mem_alloc dynamically allocate 4 bytes aligned memory in heap)
	 * and by calling 'tcb_resume', the sp will be correctly set
	 */
	tcb->stack_bottom = (uint32_t)mem_alloc(tcb->stack_size);
	if(tcb->stack_bottom == 0 || tcb->stack_bottom % 4){
		tcb->state = destroyed;
		return -1;
	}
	tcb->stack_bottom += tcb->stack_size;

	tcb->registers[psp] = tcb->stack_bottom;

	if(period > MAX_PERIOD_NUM){
		tcb->sched_ctx.period = MAX_PERIOD_NUM;
	}
	else{
		tcb->sched_ctx.period = period;
		if(tcb->sched_ctx.period == 0){
			tcb->sched_ctx.period++;
		}
	}
	if(budget > tcb->sched_ctx.period){
		tcb->sched_ctx.budget = tcb->sched_ctx.period;
	}
	else{
		tcb->sched_ctx.budget = budget;
		if(tcb->sched_ctx.budget == 0){
			tcb->sched_ctx.budget++;
		}
	}
	/* 
	 * set the first refill of this task. inited thread's threshold will be set to 0
	 * which means on the first time function 'schedule' notices this task, this task
	 * will be scheduled immediately. (but it still relates to its priority)
	 */
	tcb->sched_ctx.refills[0].threshold = 0;
	tcb->sched_ctx.refills[0].amount = tcb->sched_ctx.budget;
	tcb->sched_ctx.front = 0;
	tcb->sched_ctx.real = tcb->sched_ctx.front + 1;
	return 0;
}

void tcb_destroy(struct tcb *tcb){
	if(tcb->state == inactive){
		tcb->state = destroyed;
		if(!tcb->is_queued){
			// if this tcb still exists in a queue, it's stack wont be freed
			// until it is removed from the queue
			tcb_free_stack(tcb);
		}
	}
	return;
}


void tcb_free_stack(struct tcb *tcb){
	if(tcb_valid_stack(tcb)){
		mem_free((void *)(tcb->stack_bottom - tcb->stack_size));
		// make sure the tcb's stack becomes invalid after tcb_free_stack
		tcb->stack_bottom = 0;
	}
	return;
}

void tcb_suspend(struct tcb *tcb){
	if(tcb->state == restart || tcb->state == running){
		tcb->state = inactive;
	}
	return;
}

bool tcb_valid_stack(struct tcb *tcb){
	return !(tcb->stack_size % TSTACK_BLOCK) 
				&& tcb->stack_size > 0
                                && (tcb->stack_bottom > tcb->stack_size) 
				&& !(tcb->stack_bottom % 4);
}

bool tcb_is_valid(struct tcb *tcb){
	return tcb->priority <= MIN_PRIORITY &&
		!(tcb->state == destroyed) &&
		tcb_valid_stack(tcb) &&
		tcb->sched_ctx.period <= MAX_PERIOD_NUM &&
		tcb->sched_ctx.period > 0 &&
		tcb->sched_ctx.budget <= tcb->sched_ctx.period &&
		tcb->sched_ctx.budget > 0;
}


uint64_t tcb_threshold(struct tcb *tcb){
	return tcb->sched_ctx.refills[tcb->sched_ctx.front].threshold;
}

uint32_t *tcb_registers(struct tcb *tcb){
	return tcb->registers;
}


