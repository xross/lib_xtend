#include <platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "print.h"
#include "xtnd.h"
#include "xcore/hwtimer.h"
#include "quadflash.h"

#ifndef XTND_BASE_ADDR
#error "XTND_BASE_ADDR must be defined"
#endif

#define XTEND_IMAGE_MAX_SIZE (5000)

fl_QSPIPorts spiPort = {
    PORT_SQI_CS,
    PORT_SQI_SCLK,
    PORT_SQI_SIO,
    XS1_CLKBLK_1
};

int simpleI2c(void)
{
    hwtimer_t timer = hwtimer_alloc();

    int fail = xtend_read(spiPort, (uint8_t *) XTND_BASE_ADDR, XTEND_IMAGE_MAX_SIZE);

    if (fail)
    {
        printf("[app_test] xtend_read_flash failed %d\n", fail);
    }

    xtend_table_t tab;
    xtend_status_t irc = xtend_init((uint8_t *)XTND_BASE_ADDR, XTEND_IMAGE_MAX_SIZE, &tab);

    if (irc != XTEND_OK)
    {
        printf("[app_test] xtend_init failed rc=%d\n", (int)irc);
        return 1;
    }

    printf("[app_test] exec_ram=%p magic=0x%08lX ver=%lu count=%lu\n",
           (void*)XTND_BASE_ADDR, tab.hdr->magic, tab.hdr->version, tab.hdr->count);

    printf("[app_test] cp=%p dp=%p cp_off=0x%08lX dp_off=0x%08lX\n",
           tab.cp_base,tab.dp_base, tab.hdr->cp_off, tab.hdr->dp_off);

    xtend_fn_t fn_port_toggle_bit = (xtend_fn_t)xtend_find(&tab, "xtend_port_toggle_bit");
    xtend_fn_t fn_i2c_read = (xtend_fn_t)xtend_find(&tab, "xtend_i2c_read");

    if (!fn_port_toggle_bit)
    {
        printf("[app_test] Failed to resolve xtend function port_toggle_bit=%p\n", (void*)fn_port_toggle_bit);
        return 1;
    }

    if (!fn_i2c_read)
    {
        printf("[app_test] Failed to resolve xtend function i2c_read=%p\n", (void*)fn_i2c_read);
        return 1;
    }

    int ret = xtend_call(&tab, (void*)fn_i2c_read, 0x60, 0);
    printintln(ret);

    int ledNum = 0;
    while (1)
    {
        for(int pinVal = 0; pinVal < 2; pinVal++)
        {
            hwtimer_delay(timer, 10000000);

            /* Call the xtender function to toggle a bit on the port */
            (void)xtend_call(&tab, (void*)fn_port_toggle_bit, ledNum, 0);
        }

        ledNum++;
        if(ledNum > 3)
            ledNum = 0;

    }

    return 0;
}
