#include "platform.h"

port p = XS1_PORT_1A;

int xtend_port_output(unsigned a, unsigned b)
{
    /* If ctors not run (port enabled) this will trap */
    p <: a;

    return a+b;
}
