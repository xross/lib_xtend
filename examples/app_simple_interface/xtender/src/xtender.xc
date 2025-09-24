

#include <print.h>
#include <platform.h>

out port p = XS1_PORT_4C;

interface pll_ref_if
{
    void toggle(int);
    void exit();
};

[[distributable]]
void testTask(server interface pll_ref_if i_pll_ref, out port p_pll_ref)
{
    static unsigned pinVal= 0;

    while(1)
    {
        select
        {
            case i_pll_ref.toggle(int bit):

                int toggleMask = 1 << bit;
                pinVal ^= toggleMask;
                p_pll_ref <: pinVal;
                break;
            case i_pll_ref.exit():
                return;
        }
    }
}

int xtend_port_toggle_bit(unsigned bit, unsigned x)
{
    interface pll_ref_if i_pll_ref;

    par
    {
        testTask(i_pll_ref, p);
        {
            i_pll_ref.toggle(bit);
            i_pll_ref.exit();
        }
    }

    return (int)(bit + x);
}



