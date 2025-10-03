// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <print.h>
#include "xtnd.h"

#include "xtend_blob.h"
#include "xtnd_test_config.h"

#ifndef XTND_BASE_ADDR
#error "XTND_BASE_ADDR must be defined"
#endif

int main(void)
{
    xtend_table_t tab;
    int memSize = XTND_TEST_BLOB_ALLOC_SIZE;

    memcpy((void*)XTND_BASE_ADDR, xtend_blob_bin, xtend_blob_bin_len);

    xtend_status_t irc = xtend_init((uint8_t*)XTND_BASE_ADDR, memSize, &tab);

    if (irc != XTEND_OK)
    {
        printf("[SIM] xplug_table_init failed rc=%d\n", (int)irc);
        return 1;
    }

        xtend_fn_t fn_port_output = (xtend_fn_t)xtend_find(&tab, "xtend_port_output");

    if (!fn_port_output)
    {
        printf("[SIM] Failed to resolve plugin function port_output=%p\n", (void*)fn_port_output);
        return 1;
    }

        int rc_port_output = xtend_call(&tab, (void*)fn_port_output, 4, 5);

        if (rc_port_output == 9)
        printstrln("PASS");
    else
        printintln(rc_port_output);

    return 0;
}
