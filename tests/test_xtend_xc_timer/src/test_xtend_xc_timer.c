// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <print.h>
#include "xtnd.h"

#include "xtend_blob.h"

int main(void)
{
    xtend_table_t tab;

    xtend_status_t irc = xtend_init(xtend_blob_bin, &tab);

    if (irc != XTEND_OK)
    {
        printf("[SIM] xtend_table_init failed rc=%d\n", (int)irc);
        return 1;
    }

    xtend_fn_t fn_delay   = (xtend_fn_t)xtend_find(&tab, "xtend_delay");

    if (!fn_delay)
    {
        printf("[SIM] Failed to resolve plugin function xtend_delay=%p\n", (void*)fn_delay);
        return 1;
    }

    int delay = 100;
    int rc_delay = xtend_call(&tab, (void*)fn_delay, delay, 0);

    if(rc_delay == (delay))
        printstrln("PASS");
    else
        printintln(rc_delay);
    return 0;
}
