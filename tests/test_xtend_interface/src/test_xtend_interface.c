// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <print.h>

#include "xtnd.h"
#include "xtend_blob.h"

#ifndef XTEND_IMAGE_ADDR
#error "XTEND_IMAGE_ADDR must be defined"
#endif

int main(void)
{
    /* Move the blob to a known location in memory */
    memcpy((void*)XTEND_IMAGE_ADDR, xtend_blob_bin, xtend_blob_bin_len);

    xtend_table_t tab;

    xtend_status_t irc = xtend_init((uint8_t *)XTEND_IMAGE_ADDR, &tab);

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

    int rc_add   = xtend_call(&tab, (void*)fn_add, 4, 5);
    int rc_sub   = xtend_call(&tab, (void*)fn_sub, 4, 5);

    if((rc_add == 9) && (rc_sub == -1))
    {
        printstrln("PASS");
    }
    else
    {
        printintln(rc_add);
        printintln(rc_sub);
    }
    return 0;
}
