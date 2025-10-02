
#include "xtnd.h"

int xc_call(xtend_table_t &tab, unsigned * unsafe fn_delay)
{
    timer t;
    int fail = 1;
    int run = 0;
    unsigned time, oldTime;
    t :> oldTime;
    const int longDelay = 600;
    const int shortDelay = 100;

    t when timerafter(oldTime + longDelay) :> time;
#if 0
    select
    {
        case t when timerafter(oldTime + longDelay) :> time:
            break;

        default:

            if(run == 0)
            unsafe
            {
                int rc_delay = xtend_call(&tab, (void*)fn_delay, shortDelay, 0);
                run = 1;
            }
            break;
    }

#endif

# if 0
            unsafe
            {
                int rc_delay = xtend_call(&tab, (void*)fn_delay, shortDelay, 0);
                run = 1;
            }

    // time should be oldTime + longDelay (with some tolerance)
    printf("oldTime = %u, time = %u\n", oldTime, time);
#endif

return fail;
}

