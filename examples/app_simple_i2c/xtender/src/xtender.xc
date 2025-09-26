

#include <platform.h>

#include "i2c.h"
#include <print.h>

port p_i2c_scl = XS1_PORT_1N;
port p_i2c_sda = XS1_PORT_1O;

#define AIC3204_I2C_DEVICE_ADDR 0x18

int xtend_i2c_read(unsigned reg, unsigned data)
{
    i2c_master_if i2c[1];
    uint8_t readVal = 0xBA;
    i2c_regop_res_t result;

    par
    {
        [[distribute]]
        i2c_master(i2c, 1, p_i2c_scl, p_i2c_sda, 10);
        {
            readVal = i2c[0].read_reg(AIC3204_I2C_DEVICE_ADDR, reg, result);
            i2c[0].shutdown();
        }
    }

    return readVal;
}

on tile[0]: out port p = XS1_PORT_4C;

/* Additional interface and task(s) for port toggling.
 * Note, this is much more complext than required but demonstates what can be done.
 */

interface port_toggle_if
{
    void toggle(int);
    void exit();
};

[[distributable]]
void portToggler(server interface port_toggle_if i_port_toggle, out port p)
{
    static unsigned pinVal = 0;

    while(1)
    {
        select
        {
            case i_port_toggle.toggle(int bit):
                int toggleMask = 1 << bit;
                pinVal ^= toggleMask;
                p <: pinVal;
                break;
            case i_port_toggle.exit():
                return;
        }
    }
}

void toggleClient(client interface port_toggle_if i_port_toggle, unsigned bit)
{
    i_port_toggle.toggle(bit);
    i_port_toggle.exit();
}

int xtend_port_toggle_bit(unsigned bit, unsigned x)
{
    interface port_toggle_if i_port_toggle;

    par
    {
        portToggler(i_port_toggle, p);
        toggleClient(i_port_toggle, bit);
    }

    return (int)(bit + x);
}



