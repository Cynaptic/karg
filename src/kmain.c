#include <init.h>
#include <kalloc.h>
#include <log.h>
#include <module.h>
#include <sched.h>

void init(void);

static proc_t init_proc;

MODULE_NAME("main");

[[noreturn]] void kmain(void) {
    init_modules();

    proc_init(&init_proc, init, 0, nullptr, 0, nullptr);
    sched_update_state(&init_proc, PROC_STATE_READY);

    sched_start();

    unreachable();
}
