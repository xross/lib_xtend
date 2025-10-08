// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <print.h>
#include "xtnd.h"

#include "xtnder_blob.h"
#include "xtnd_test_config.h"

#ifndef XTND_BASE_ADDR
#error "XTND_BASE_ADDR must be defined"
#endif

int main(void)
{
    xtnd_table_t tab;
    int memSize = XTND_TEST_BLOB_ALLOC_SIZE;

    memcpy((void*)XTND_BASE_ADDR, xtnder_blob_bin, xtnder_blob_bin_len);

    xtnd_status_t irc = xtnd_init((uint8_t*)XTND_BASE_ADDR, memSize, &tab);

    if (irc != XTND_OK)
    {
        printf("[SIM] xplug_table_init failed rc=%d\n", (int)irc);
        return 1;
    }

        xtnd_fn_t fn_port_output = (xtnd_fn_t)xtnd_find(&tab, "xtnd_port_output");

    if (!fn_port_output)
    {
        printf("[SIM] Failed to resolve plugin function port_output=%p\n", (void*)fn_port_output);
        return 1;
    }

        int rc_port_output = xtnd_call(&tab, (void*)fn_port_output, 4, 5);

        if (rc_port_output == 9)
        printstrln("PASS");
    else
        printintln(rc_port_output);

    return 0;
}
