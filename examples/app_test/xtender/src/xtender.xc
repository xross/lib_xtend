


#include <print.h>
#include <platform.h>

out port p = XS1_PORT_4C;

int g_pinVal = 0;
int g_test;

int xtend_port_toggle(int a, int b)
{
    g_pinVal = ~g_pinVal;
    p <: g_pinVal;

    return g_test++;
}


#if 0

interface pll_ref_if
{
    void toggle();
};

[[distributable]]
void testTask(server interface pll_ref_if i_pll_ref, out port p_pll_ref)
{
    static unsigned pinVal= 0;

    while(1)
    {
        select
        {
            case i_pll_ref.toggle():
                pinVal = ~pinVal;
                 p_pll_ref <: pinVal;
                break;
        }
    }
}

void user(client interface pll_ref_if i_pll_ref)
{
    i_pll_ref.toggle();
}

int user_entry(unsigned a0, unsigned a1, unsigned a2, unsigned a3, void * unsafe ctx)
{
    interface pll_ref_if i_pll_ref;

    par
    {
        testTask(i_pll_ref, p);
        user(i_pll_ref);
    }


    return (int)(a0 + a1);
}

#endif

