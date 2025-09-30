
#include <xtnd.h>

int xc_call(xtend_table_t &tab, unsigned * unsafe fn_add, unsigned * unsafe fn_sub, int a, int b)
{
    unsafe
    {
        int rc_add   = xtend_call(&tab, (void * unsafe)fn_add, 4, 5);
        int rc_sub   = xtend_call(&tab, (void * unsafe)fn_sub, 4, 5);

        if((rc_add == 9) && (rc_sub == -1))
        {
            return 0;
        }
    }

    return 1;
}
