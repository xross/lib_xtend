// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <print.h>
#include "xtnd.h"

#include "xtnder_blob.h" /* TODO rename to xtnder_blob.h after blob generation */
#include "xtnd_test_config.h"

#ifndef XTND_BASE_ADDR
#error "XTND_BASE_ADDR must be defined"
#endif

int main(void)
{
    xtnd_table_t tab;
    int memSize = XTND_TEST_BLOB_ALLOC_SIZE;

    /* Move the blob to a known location in memory */
    memcpy((void*)XTND_BASE_ADDR, xtnder_blob_bin, xtnder_blob_bin_len);

    xtnd_status_t irc = xtnd_init((uint8_t *)XTND_BASE_ADDR, memSize, &tab);

    if (irc != XTND_OK)
    {
        printf("[SIM] xtnd_table_init failed rc=%d\n", (int)irc);
        return 1;
    }

    xtnd_fn_t fn_add   = (xtnd_fn_t)xtnd_find(&tab, "xtnd_add");
    xtnd_fn_t fn_sub   = (xtnd_fn_t)xtnd_find(&tab, "xtnd_sub");

    if (!fn_add)
    {
        printf("[SIM] Failed to resolve plugin function plugin_add=%p\n", (void*)fn_add);
        return 1;
    }

    if (!fn_sub)
    {
        printf("[SIM] Failed to resolve plugin function plugin_sub=%p\n", (void*)fn_sub);
        return 1;
    }

    int rc_add   = xtnd_call(&tab, (void*)fn_add, 4, 5);
    int rc_sub   = xtnd_call(&tab, (void*)fn_sub, 4, 5);

    if((rc_add == 9) && (rc_sub == -1))
        printstrln("PASS");
    else
    {
        printintln(rc_add);
        printintln(rc_sub);
    }
    return 0;
}
