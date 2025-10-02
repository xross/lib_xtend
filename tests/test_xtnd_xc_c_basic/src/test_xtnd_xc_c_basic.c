// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* Test xtender usage from XC */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <print.h>
#include "xtnd.h"

#include "xtend_blob.h"

int xc_call(xtend_table_t *tab, xtend_fn_t fn_add, xtend_fn_t fn_sub, int a, int b);

int main(void)
{
    xtend_table_t tab;
    int memSize = xtend_blob_bin_len + 128;
    uint8_t *blob = malloc(memSize);

    memcpy(blob, xtend_blob_bin, xtend_blob_bin_len);

    xtend_status_t irc = xtend_init(blob, memSize, &tab);

    if (irc != XTEND_OK)
    {
        printf("[SIM] xtend_table_init failed rc=%d\n", (int)irc);
        return 1;
    }

    xtend_fn_t fn_add   = (xtend_fn_t)xtend_find(&tab, "xtend_add");
    xtend_fn_t fn_sub   = (xtend_fn_t)xtend_find(&tab, "xtend_sub");

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
