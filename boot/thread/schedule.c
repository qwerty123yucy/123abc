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

static bool priority_cmp_ready(struct tcb *a, struct tcb *b){
        return (a->priority == b->priority) ? (a < b) : (a->priority < b->priority);
}

static bool priority_cmp_wait(struct tcb *a, struct tcb *b){
        uint64_t threshold_a = tcb_threshold(a);
        uint64_t threshold_b = tcb_threshold(b);
        return (threshold_a == threshold_b) ? (a < b) : (threshold_a < threshold_b);
}


static struct tcb_queue {
	/* 
	 * use 'small top heap' to implement priority queue
	 * priority queue starts at index '1' to ease operation on heap
	 */ 
	struct tcb *queue[MAX_QUEUE_SIZE + 1];
	uint32_t num;
	bool (*priority_cmp)(struct tcb *a, struct tcb *b);
	
} g_ready_queue = {.num = 1, .priority_cmp = priority_cmp_ready}, 
	g_wait_queue = {.num = 1, .priority_cmp = priority_cmp_wait};


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
				|| (!tcb_is_valid(&(g_tasks.tasks[index])) 
					&& !g_tasks.tasks[index].is_queued)){
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


static int enqueue(struct tcb *tcb, struct tcb_queue *queue){
	if(queue->num >= MAX_QUEUE_SIZE + 1){
		return -1;
	}
	uint32_t index = queue->num;
	queue->queue[queue->num++] = tcb;
	for(uint32_t i = index / 2;i > 0;i /= 2){
		// g_ready_queue uses tcb's priority as priority
		// g_wait_queue uses threshold as priority
		// to ensure task with smaller priority value will be served earlier
		// and task with smaller threshold will be added into g_ready_queue as soon as possible
		if(2 * i + 1 <= index && 
				queue->priority_cmp(queue->queue[2 * i + 1], 
					queue->queue[i])){
			
			struct tcb *tmp = queue->queue[i];
			queue->queue[i] = queue->queue[2 * i + 1];
			queue->queue[2 * i + 1] = tmp;
		}
		if(2 * i <= index &&
				queue->priority_cmp(queue->queue[2 * i], 
					queue->queue[i])){
			
			struct tcb *tmp = queue->queue[i];
			queue->queue[i] = queue->queue[2 * i];
			queue->queue[2 * i] = tmp;
		}
	}
	tcb->is_queued = true;
	
	return 0;
	
}

static int enqueue_ready(struct tcb *tcb){
	return enqueue(tcb, &g_ready_queue);
}

static int enqueue_wait(struct tcb *tcb){
	return enqueue(tcb, &g_wait_queue);
}


static struct tcb *dequeue(struct tcb_queue *queue){
	if(queue->num <= 1){
		return NULL;
	}
	struct tcb *front = queue->queue[1];
	queue->queue[1] = queue->queue[--(queue->num)];
	
	if(queue->num > 2){
		struct tcb *top = queue->queue[1];
		uint32_t i = 1;
		while(i * 2 < queue->num){
			uint32_t k = i * 2;
			if((i * 2 + 1) < queue->num 
					&& !queue->priority_cmp(queue->queue[2 * i], 
						queue->queue[2 * i + 1])){
				k++;
			}
			if(queue->priority_cmp(queue->queue[k], top)){
				queue->queue[i] = queue->queue[k];
				i = k;
			}
			else{
				break;
			}
		}
		queue->queue[i] = top;
	}
	front->is_queued = false;
	
	return front;
}

static struct tcb *dequeue_ready(){
	return dequeue(&g_ready_queue);
}

static struct tcb *dequeue_wait(){
	return dequeue(&g_wait_queue);
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
	if(index >= g_tasks.num || g_tasks.tasks[index].is_queued 
			|| !g_tasks.tasks[index].state == inactive){
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
			if(top_ready->state == running 
					|| top_ready->state == restart){
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
	struct tcb tcb;
	// create the first tcb 'main_loop'
	// this task can run 1 systick within 1 systick
	// stack_size: 4kB
	// priority: 0 (highest)
	// entry: main_loop (in utils/shell.c)
	tcb_init(&tcb, 1, 2, 4, 0, (uint32_t)main_loop);

	int index = tcb_add(tcb);
	if(index >= 0){
		tcb_activate((uint32_t)index);
	}
	return;
}



