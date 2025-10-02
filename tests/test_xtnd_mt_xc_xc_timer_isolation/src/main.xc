
#include <platform.h>
#include <stddef.h>

#ifndef THREAD_NUM
#error "THREAD_NUM not defined"
#endif

int test_xtend_xc_timer_isolation();

void dummyThread()
{
    unsigned x = 0;
    x++;
}

int main()
{
    par
    {
        on tile[0]: test_xtend_xc_timer_isolation();
#if THREAD_NUM > 0
        on tile[0]: dummyThread();
#endif
#if THREAD_NUM > 1
        on tile[0]: dummyThread();
#endif
#if THREAD_NUM > 2
        on tile[0]: dummyThread();
#endif
#if THREAD_NUM > 3
    #error
#endif
    }

    return 0;
}
