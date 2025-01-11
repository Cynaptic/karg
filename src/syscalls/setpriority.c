#include <trap.h>
#include <proc.h>
#include <sched.h>

proc_t *find_proc_by_pid(pid_t pid) {
    proc_t *proc;

    list_for_each_entry(&all_proc_list, node, proc) {
        if (proc->pid == pid) {
            return proc;
        }
    }

    return NULL;
}



isize sys_setpriority(const trapframe_t *frame) {
    pid_t pid = frame->a0;
    int priority = frame->a1;

    if (priority < PRIORITY_MIN || priority > PRIORITY_MAX) {
        return -1;
    }

    proc_t *target_proc = find_proc_by_pid(pid);
    if (!target_proc) {
        return -1;
    }

    target_proc->priority = priority;

    if (target_proc == curr_proc) {
        sched_resched();
    }

    return 0;
}
