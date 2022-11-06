#ifndef	__SCHEDULE_H
#define __SCHEDULE_H

#include <stdint.h>
#include <stdbool.h>
#include <thread/tcb.h>

#define MAX_TASK_NUM            32U
#define MAX_QUEUE_SIZE          32U

int tcb_add(struct tcb tcb);
int tcb_activate(uint32_t index);
struct tcb *schedule();
void init_task();

#endif
