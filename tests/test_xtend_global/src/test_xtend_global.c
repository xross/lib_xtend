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

    xtend_fn_t fn_global_add = (xtend_fn_t)xtend_find(&tab, "xtend_global_add");
    if (!fn_global_add)
    {
        printf("[SIM] Failed to resolve plugin function xtend_global_add=%p\n", (void*)fn_global_add);
        return 1;
    }

    int rc_global_add = xtend_call(&tab, (void*)fn_global_add, 4, 5);

    if (rc_global_add == 1009)
        printstrln("PASS");
    else
        printintln(rc_global_add);

    return 0;
}
