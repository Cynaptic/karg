#pragma once

#include <proc.h>

#define PRIORITY_MIN 0
#define PRIORITY_MAX 10
extern list_node_t all_proc_list;
extern proc_t *curr_proc;

void sched_update_state(proc_t *proc, proc_state_t state);
[[noreturn]] void sched_start(void);
void sched_resched(void);
int compare_proc_priority(const void *a, const void *b);
proc_t *find_proc_by_pid(pid_t pid);