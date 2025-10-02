
#include <xs1.h>

int xtend_delay(unsigned a, unsigned b)
{
    timer tA, tB;
    unsigned timeA, timeB;
    tA:> timeA;
    tB:> timeB;

    // XC will use a single timer for this pattern
    select
    {
        case tA when timerafter(timeA + a) :> void:
            return a;

        case tB when timerafter(timeB + b) :> void:
            return b;
    }

    return 1;
}

