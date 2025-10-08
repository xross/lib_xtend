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

#define XTEND_IMAGE_MAX_SIZE (2000)

fl_QSPIPorts spiPort = {
    PORT_SQI_CS,
    PORT_SQI_SCLK,
    PORT_SQI_SIO,
    XS1_CLKBLK_1
};

int main(void)
{
    hwtimer_t timer = hwtimer_alloc();

    int fail = xtnd_read(spiPort, (uint8_t *)XTND_BASE_ADDR, XTEND_IMAGE_MAX_SIZE);

    if (fail)
    {
        printf("[app_test] xtnd_read_flash failed %d\n", fail);
    }

    xtnd_table_t tab;
    xtnd_status_t irc = xtnd_init((uint8_t *)XTND_BASE_ADDR, XTEND_IMAGE_MAX_SIZE, &tab);

    if (irc != XTEND_OK)
    {
        printf("[app_test] xtnd_init failed rc=%d\n", (int)irc);
        return 1;
    }

    printf("[app_test] exec_ram=%p magic=0x%08lX ver=%lu count=%lu\n",
           (void*)XTND_BASE_ADDR, tab.hdr->magic, tab.hdr->version, tab.hdr->count);

    printf("[app_test] cp=%p dp=%p cp_off=0x%08lX dp_off=0x%08lX\n",
           tab.cp_base,tab.dp_base, tab.hdr->cp_off, tab.hdr->dp_off);

    xtnd_fn_t fn_port_toggle_bit = (xtnd_fn_t)xtnd_find(&tab, "xtnd_port_toggle_bit");

    if (!fn_port_toggle_bit)
    {
        printf("[app_test] Failed to resolve xtend function port_toggle_bit=%p\n", (void*)fn_port_toggle_bit);
        return 1;
    }

    int ledNum = 0;
    while (1)
    {
        for(int pinVal = 0; pinVal < 2; pinVal++)
        {
            hwtimer_delay(timer, 10000000);

            /* Call the xtender function to toggle a bit on the port */
            (void)xtnd_call(&tab, (void*)fn_port_toggle_bit, ledNum, 0);
        }

        ledNum++;
        if(ledNum > 3)
            ledNum = 0;

    }

    return 0;
}
