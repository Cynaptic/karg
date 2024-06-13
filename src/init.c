#include <init.h>

#include <log.h>
#include <module.h>

extern init_t _pre_init_start, _pre_init_end;
extern init_t _init_start, _init_end;
extern init_t _post_init_start, _post_init_end;

MODULE_NAME("init");

i32 init_modules(void) {
    log_info("entering the pre-initialization phase");
    for (init_t *init_ptr = &_pre_init_start; init_ptr < &_pre_init_end;
         init_ptr++) {
        i32 res = init_ptr->init();
        log_info("initialized %s", init_ptr->name);
        if (res < 0)
            return res;
    }

    log_info("entering the initialization phase");
    for (init_t *init_ptr = &_init_start; init_ptr < &_init_end; init_ptr++) {
        i32 res = init_ptr->init();
        log_info("initialized %s", init_ptr->name);
        if (res < 0)
            return res;
    }

    log_info("entering the post-initialization phase");
    for (init_t *init_ptr = &_post_init_start; init_ptr < &_post_init_end;
         init_ptr++) {
        i32 res = init_ptr->init();
        log_info("initialized %s", init_ptr->name);
        if (res < 0)
            return res;
    }

    return 0;
}
