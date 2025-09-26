
#include <xs1.h>
#include <platform.h>

int simpleI2c(void);

on tile[1]: out port p_codecRst  = PORT_CODEC_RST_N;


void codecRst(void)
{
    /* Take the CODEC out of reset such that we can access it via I2C */
    const int codecRstMask = 8;
    p_codecRst <: codecRstMask;
    while(1);
}

int main()
{
    par
    {
        on tile[0]: simpleI2c();
        on tile[1]: codecRst();
    }
    return 0;
}
