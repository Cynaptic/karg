#include <sched.h>
#include <arch.h>
#include <module/init.h>
#include <module/module.h>
#include <priority_queue.h>

MODULE_NAME("sched");

list_node_t all_proc_list = { &all_proc_list, &all_proc_list };

static priority_queue_t ready_queue;
static proc_t idle_proc;
static timer_t timer;

proc_t *curr_proc = nullptr;

static void idle_task() {
    while (true) {
        wait_for_intr();
    }
}

static i32 init(void) {
    priority_queue_init(&ready_queue, compare_proc_priority);

    i32 res = proc_init(&idle_proc, idle_task, PROC_FLAG_KERN, nullptr, 0, nullptr);
    if (res < 0)
        return res;
    idle_proc.priority = PRIORITY_MIN;
    sched_update_state(&idle_proc, PROC_STATE_READY);

    timer_init(&timer);

    return 0;
}

module_post_init(init);

void sched_update_state(proc_t *proc, proc_state_t state) {
    if (proc->state == state)
        return;

    switch (proc->state) {
    case PROC_STATE_READY:
        priority_queue_remove(&ready_queue, &proc->node);
        break;
    case PROC_STATE_SLEEP:
        timer_cancel(&proc->timer);
        break;
    default:
        break;
    }

    proc->state = state;

    switch (state) {
    case PROC_STATE_READY:
        priority_queue_push(&ready_queue, &proc->node);
        break;
    case PROC_STATE_CURR:
        curr_proc = proc;
        break;
    case PROC_STATE_WAIT_SEM:
    case PROC_STATE_WAIT_CHILD:
    case PROC_STATE_SLEEP:
        if (proc == curr_proc)
            sched_resched();
        break;
    case PROC_STATE_ZOMBIE:
        if (proc == curr_proc) {
            curr_proc = nullptr;
            sched_resched();
        }
        break;
    default:
        break;
    }
}

static void sched_preempt(void *data) {
    (void)data;
    timer_wait(&timer, SCHED_TIMESLICE, sched_preempt, nullptr);
    sched_resched();
}

[[noreturn]] void sched_start(void) {
    timer_wait(&timer, SCHED_TIMESLICE, sched_preempt, nullptr);
    sched_resched();
    unreachable();
}

void sched_resched(void) {
    proc_t *old_proc = curr_proc;
    if (old_proc && old_proc->state == PROC_STATE_CURR)
        sched_update_state(old_proc, PROC_STATE_READY);

    proc_t *new_proc = list_entry(priority_queue_top(&ready_queue), proc_t, node);
    if (!new_proc) {
        new_proc = &idle_proc;
    } else {
        priority_queue_pop(&ready_queue);
    }

    sched_update_state(new_proc, PROC_STATE_CURR);
    proc_ctx_sw(old_proc, new_proc);
}

int compare_proc_priority(const void *a, const void *b) {
    const proc_t *proc_a = (const proc_t *)a;
    const proc_t *proc_b = (const proc_t *)b;
    return proc_b->priority - proc_a->priority;
}
