
#include <xs1.h>

int xtend_delay(unsigned a, unsigned b)
{
    timer t;
    unsigned time;
    t:> time;

    t when timerafter(time + a) :> time;

    return time;
}

