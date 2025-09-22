

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

