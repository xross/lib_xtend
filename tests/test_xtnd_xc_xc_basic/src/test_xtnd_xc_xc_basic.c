// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* Test xtender usage from XC */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <print.h>
#include "xtnd.h"

#include "xtnder_blob.h"
#include "xtnd_test_config.h"

int xc_call(xtnd_table_t *tab, xtnd_fn_t fn_add, xtnd_fn_t fn_sub, int a, int b);

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

    int fail = xc_call(&tab, fn_add, fn_sub, 4, 5);

    if(fail)
    {
        printintln(fail);
    }
    else
    {
        printstrln("PASS");
    }

    return fail;
}
