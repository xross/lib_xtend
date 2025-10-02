
#include <platform.h>
#include <stddef.h>

#ifndef THREAD_NUM
#error "THREAD_NUM not defined"
#endif

int test_xtnd_mt_c_xc_timer();

void dummyThread()
{
    unsigned x = 0;
    x++;
}

int main()
{
    par
    {
        on tile[0]: test_xtnd_mt_c_xc_timer();
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
