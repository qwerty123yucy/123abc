#ifndef	__TCB_H
#define __TCB_H
#include <stdint.h>
#include <stdbool.h>

#define TSTACK_BLOCK			(1024U)
#define DEFAULT_TSTACK_BLOCKS		(4U)	// each thread have 4k private stack by default
#define DEFAULT_TSTACK_SIZE		(TSTACK_BLOCK * DEFAULT_TSTACK_BLOCKS)
#define MAX_TSTACK_BLOCKS		(8U)

#define USER_REGISTER_NUM		(17U)	// total of 17 registers need to be saved and recovered when context switching

#define MAX_PERIOD_NUM			(32U)	// thread can be scheduled for 'budget' times in total of 'period' schedules
#define MAX_REFILL_NUM			MAX_PERIOD_NUM

#define MIN_PRIORITY			(32U)	// thread with smaller priority number has higher priority
enum user_registers {r0 = 0U, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, psp, lr, pc, apsr};

enum tcb_state {inactive = 0U, restart, running, waiting, destroyed};

struct schedule_context {
	// thread can be scheduled for 'budget' times in total of 'period' schedules
	uint8_t budget;
	uint8_t period;
	struct {
		// indicates when this task can be scheduled  and the amount
		uint64_t threshold;
		uint32_t amount;		
	} refills[MAX_REFILL_NUM];
	// the 'refills' is treated as a circular buffer whose head and tail is pointed by 'front(=head)' and 'real(=tail+1)'
	uint8_t front;
	uint8_t real;

}; 

struct tcb {
	uint32_t registers[USER_REGISTER_NUM];
	uint32_t state;
	uint32_t priority;
	uint32_t stack_size;
	// each tcp are allocated a stack field
	uint32_t stack_bottom;
	bool is_queued;
	struct schedule_context sched_ctx;
};

int tcb_init(struct tcb *tcb,
                uint8_t budget, uint8_t period,
                uint8_t stack_blocks, uint8_t priority,
                uint32_t entry);

void tcb_destroy(struct tcb *tcb);
void tcb_suspend(struct tcb *tcb);
bool tcb_is_valid(struct tcb *tcb);
bool tcb_valid_stack(struct tcb *tcb);
void tcb_free_stack(struct tcb *tcb);
uint64_t tcb_threshold(struct tcb *tcb);


#endif



