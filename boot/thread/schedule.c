#include <thread/tcb.h>
#include <stdint.h>
#include <stddef.h>
#include <utils/shell.h>
#include <thread/schedule.h>
#include <utils/io.h>
/* 
 * all task should first be inserted into g_tasks, 
 * only after which can they be added into queues.
 *
 * g_tasks may also contains destroyed tasks, but they will be covered by new tasks
 * after they are turely removed from queues.
 */
static struct {
	struct tcb tasks[MAX_TASK_NUM];
	uint32_t num;
} g_tasks = {.num = 0};


/*
 * g_ready_queue: contains all runnable tasks' tcbs sorted by priority
 * 
 * g_wait_queue: once a runnable task exhausts its budget, 
 * and g_tick reaches none of its refills's threshold, 
 * the task will be put into g_wait_queue by function 'schedule'.
 * tcbs in g_wait_queue are sorted by their threshold
 * once the g_tick reaches a waiting task's threshold, this task will be put into g_ready_queue
 * by function 'schedule'
 *
 * some tcbs in g_ready_queue and g_wait_queue are even not 'ready to run' or 'waiting', since they may be destroyed 
 * or suspended by operation directly modifying the g_tasks 
 * (items in both queue are pointers to items in g_tasks),
 * however, when function 'schedule' notices them, they will be removed directly from queue 
 */
 
static struct {
	/* 
	 * use 'small top heap' to implement priority queue
	 * priority queue starts at index '1' to ease operation on heap
	 */ 
	struct tcb *queue[MAX_QUEUE_SIZE + 1];
	uint32_t num;
} g_ready_queue = {.num = 1}, g_wait_queue = {.num = 1};


// records total schedule times since boot, 
// after exiting from a task, this value increases by 1.
uint64_t g_tick = 0;

struct tcb *g_current_tcb = NULL;


// add a task's tcb into g_tasks and return the task num;
// all tcb to be inserted into g_tasks need to be inactive and valid
int tcb_add(struct tcb tcb){
	if(!(tcb_is_valid(&tcb)) || !(tcb.state == inactive) || tcb.is_queued){
		// only tcb who is marked as inactive can be inserted into g_tasks
		return -1;
	}
	uint32_t index = 0;
	for(;index <= g_tasks.num;index++){
		if(index == g_tasks.num 
				|| (!tcb_is_valid(&(g_tasks.tasks[index])) && !g_tasks.tasks[index].is_queued)){
			// invalid tasks won't be covered by new tasks,
			// unless they are turely removed from queues (dequeue will unset tcbs' is_queued flag)
			break;
		}
	}
	if(index == g_tasks.num && g_tasks.num >= MAX_TASK_NUM){
		// no enough space for adding task
		return -1;
	}
	else{
		tcb_init_sched_ctx(&tcb);
		g_tasks.tasks[index] = tcb;
		if(index == g_tasks.num){
			g_tasks.num++;
		}
		// task's index in g_tasks.tasks is its task id
		return (int)index;
	}
}


static int enqueue_ready(struct tcb *tcb){
	if(g_ready_queue.num >= MAX_QUEUE_SIZE + 1){
		return -1;
	}
	uint32_t index = g_ready_queue.num;
	g_ready_queue.queue[g_ready_queue.num++] = tcb;
	for(uint32_t i = index / 2;i > 0;i /= 2){
		// g_ready_queue using tcb's priority as priority 
		// to ensure task with smaller priority value will be served earlier
		if(2 * i + 1 <= index && 
				g_ready_queue.queue[i]->priority 
				> g_ready_queue.queue[2 * i + 1]->priority){
			
			struct tcb *tmp = g_ready_queue.queue[i];
			g_ready_queue.queue[i] = g_ready_queue.queue[2 * i + 1];
			g_ready_queue.queue[2 * i + 1] = tmp;
		}
		if(2 * i <= index &&
				g_ready_queue.queue[i]->priority 
				> g_ready_queue.queue[2 * i]->priority){
			
			struct tcb *tmp = g_ready_queue.queue[i];
			g_ready_queue.queue[i] = g_ready_queue.queue[2 * i];
			g_ready_queue.queue[2 * i] = tmp;
		}
	}
	tcb->is_queued = true;
	
	return 0;
	
}

static struct tcb *dequeue_ready(){
	if(g_ready_queue.num <= 1){
		return NULL;
	}
	struct tcb *front = g_ready_queue.queue[1];
	g_ready_queue.queue[1] = g_ready_queue.queue[--(g_ready_queue.num)];
	
	if(g_ready_queue.num > 2){
		struct tcb *top = g_ready_queue.queue[1];
		uint32_t i = 1;
		while(i * 2 < g_ready_queue.num){
			uint32_t k = i * 2;
			if((i * 2 + 1) < g_ready_queue.num 
					&& g_ready_queue.queue[2 * i]->priority
					>= g_ready_queue.queue[2 * i + 1]->priority){
				k++;
			}
			if(top->priority > g_ready_queue.queue[k]->priority){
				g_ready_queue.queue[i] = g_ready_queue.queue[k];
				i = k;
			}
			else{
				break;
			}
		}
		g_ready_queue.queue[i] = top;
	}
	front->is_queued = false;
	
	return front;
}

static struct tcb *dequeue_wait(){
        if(g_wait_queue.num <= 1){
                return NULL;
        }
        struct tcb *front = g_wait_queue.queue[1];
        g_wait_queue.queue[1] = g_wait_queue.queue[--(g_wait_queue.num)];

        if(g_wait_queue.num > 2){
                struct tcb *top = g_wait_queue.queue[1];
                uint32_t i = 1;
                while(i * 2 < g_wait_queue.num){
                        uint32_t k = i * 2;
                        if((i * 2 + 1) < g_wait_queue.num
                                        && tcb_threshold(g_wait_queue.queue[2 * i]) 
                                        >= tcb_threshold(g_wait_queue.queue[2 * i + 1])){
                                k++;
                        }
                        if(tcb_threshold(top) > tcb_threshold(g_wait_queue.queue[k])){
                                g_wait_queue.queue[i] = g_wait_queue.queue[k];
                                i = k;
                        }
                        else{
                                break;
                        }
                }
                g_wait_queue.queue[i] = top;
        }

	front->is_queued = false;
        return front;
}

#define wfi()	__asm__ volatile ("wfi\n" : : : "memory")

void task_test2(){
	int i = 0;
	while(1){
		if(i == 128){
			print_f("task2 i = %lx\n", (uint32_t)i);
		}
		i = (i+1) % 256;
		wfi();
	}
	

}


void task_test3(){
        int i = 0;
        while(1){
                if(i == 128){
                        print_f("task3 i = %lx\n", (uint32_t)i);
                }
                i = (i+1) % 256;
                wfi();
        }


}
void task_test4(){
        int i = 0;
        while(1){
                if(i == 128){
                        print_f("task4 i = %lx\n", (uint32_t)i);
                }
                i = (i+1) % 256;
                wfi();
        }


}


static int enqueue_wait(struct tcb *tcb){
        if(g_wait_queue.num >= MAX_QUEUE_SIZE + 1){
                return -1;
        }
        uint32_t index = g_wait_queue.num;
        g_wait_queue.queue[g_wait_queue.num++] = tcb;
        for(uint32_t i = index / 2;i > 0;i /= 2){
		// g_wait_queue using the threshold of task as priority
		// to ensure task with lower threshold will be added into g_ready_queue earlier
                if(2 * i + 1 <= index &&
                                tcb_threshold(g_wait_queue.queue[i]) 
				> tcb_threshold(g_wait_queue.queue[2 * i + 1])){
                        
			struct tcb *tmp = g_wait_queue.queue[i];
                        g_wait_queue.queue[i] = g_wait_queue.queue[2 * i + 1];
                        g_wait_queue.queue[2 * i + 1] = tmp;
                }
                if(2 * i <= index &&
                                tcb_threshold(g_wait_queue.queue[i]) 
				> tcb_threshold(g_wait_queue.queue[2 * i])){
                        
			struct tcb *tmp = g_wait_queue.queue[i];
                        g_wait_queue.queue[i] = g_wait_queue.queue[2 * i];
                        g_wait_queue.queue[2 * i] = tmp;
                }
        }

	tcb->is_queued = true;
        return 0;
}


static struct tcb *first_ready(){
	if(g_ready_queue.num > 1){
		return g_ready_queue.queue[1];
	}
	return NULL;
}

static struct tcb *first_wait(){
	if(g_wait_queue.num > 1){
		return g_wait_queue.queue[1];
	}
	return NULL;
}


// add a task in g_tasks.tasks into g_queue according to the given 'index' in g_tasks
// this function is the only way to add tcb into queues
int tcb_activate(uint32_t index){
	if(index >= g_tasks.num || g_tasks.tasks[index].is_queued || !g_tasks.tasks[index].state == inactive){
		// tcb with running or restart state is already in the g_queue
		return -1;
	}
	int ret = enqueue_ready(&(g_tasks.tasks[index]));
	if(!ret){
		// this task successfully added into global priority queue
		// so change its state
		g_tasks.tasks[index].state = restart;
	}
	return ret;
}

static bool tcb_reach_threshold(struct tcb *tcb){
	return tcb_threshold(tcb) <= g_tick;
}


static bool tcb_runnable(struct tcb *tcb){
	return (tcb->state == restart || tcb->state == running ) 
		&& tcb_reach_threshold(tcb);
}


/*
 * modify tcb's sched_ctx. 
 * sched_ctx's refill list is a circular queue
 *
 * this function decline its current refill's budget by 1 and postpone this declined 1 budget to 'period' ticks later
 * in this way, we can ensure task wont be scheduled for more than 'budget' times in total of 'period' times
 * because the '1 budget' used just now can only be used again 'period' ticks later
 * 
 * if current refill's budget turns to 0, this refill will be removed
 */
void tcb_tick_once(struct tcb *tcb){
	tcb->sched_ctx.refills[tcb->sched_ctx.front].amount--;
	tcb->sched_ctx.refills[tcb->sched_ctx.real].threshold 
		= g_tick + tcb->sched_ctx.period;
	tcb->sched_ctx.refills[tcb->sched_ctx.real].amount = 1;
	tcb->sched_ctx.real = (tcb->sched_ctx.real + 1) % MAX_REFILL_NUM;

	if(tcb->sched_ctx.refills[tcb->sched_ctx.front].amount == 0){
		tcb->sched_ctx.front = (tcb->sched_ctx.front + 1) % MAX_REFILL_NUM;
	}

	return;
}


// this function pick a runnable task with the highest priority
struct tcb *schedule(){
	struct tcb *top_ready = NULL;
	struct tcb *top_wait = NULL;
	// first check whether there is task in g_wait_queue which reached its threshold
	while((top_wait = first_wait()) != NULL){
		if(top_wait->state != waiting){
			// means this task is externally set as deatroyed or inactive
			dequeue_wait();
			if(top_wait->state == destroyed){
				tcb_free_stack(top_wait);
			}
		}
		else{
			if(tcb_reach_threshold(top_wait)){
				// try to add it into g_ready_queue
				if(!enqueue_ready(top_wait)){
					// successfully added into g_ready_queue
					// change its status and remove it from g_wait_queue
					top_wait->state = running;
					dequeue_wait();
				}
				else{
					break;
				}
			}
			else{
				break;
			}
		}
	}

	while((top_ready = first_ready()) != NULL){
		if(tcb_runnable(top_ready)){
			break;
		}
		else{
			dequeue_ready();
			if(top_ready->state == running || top_ready->state == restart){
				// this task already exhausted its budget
				top_ready->state = waiting;
				enqueue_wait(top_ready);
			}
			else if(top_ready->state == destroyed){
				tcb_free_stack(top_ready);
			}
		}
	}
	return top_ready;
}


void init_task(){
	struct tcb tcb_sh;
	struct tcb tcb_2;
	struct tcb tcb_3;
	struct tcb tcb_4;
	// create the first tcb 'main_loop'
	// this task can run 1 systick within 1 systick
	// stack_size: 4kB
	// priority: 0 (highest)
	// entry: main_loop (in utils/shell.c)
	

	tcb_init(&tcb_sh, 24, 27, 4, 0, (uint32_t)main_loop);
	tcb_init(&tcb_2, 1, 27, 1, 1, (uint32_t)task_test2);
	tcb_init(&tcb_3, 1, 27, 1, 2, (uint32_t)task_test3);
	tcb_init(&tcb_4, 1, 27, 1, 3, (uint32_t)task_test4);

	int index = tcb_add(tcb_sh);

	if(index >= 0){
		tcb_activate((uint32_t)index);
	}
	
	index = tcb_add(tcb_2);
	if(index >= 0){
                tcb_activate((uint32_t)index);
        }
	index = tcb_add(tcb_3);
        if(index >= 0){
                tcb_activate((uint32_t)index);
        }
	index = tcb_add(tcb_4);
        if(index >= 0){
                tcb_activate((uint32_t)index);
        }


}



