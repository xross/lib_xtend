
#include <xtnd.h>

int xc_call(xtend_table_t &tab, unsigned * unsafe fn_delay, int a, int b)
{
    int rc = 0;
    unsafe
    {
        int rc = xtend_call(&tab, (void * unsafe)fn_delay, a, b);
    }

    return rc;
}
