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
    int memSize =  XTND_TEST_BLOB_ALLOC_SIZE;
    memcpy((void*)XTND_BASE_ADDR, xtnder_blob_bin, xtnder_blob_bin_len);
    xtnd_status_t irc = xtnd_init((uint8_t*)XTND_BASE_ADDR, memSize, &tab);
    if (irc != XTND_OK) {
        printf("[SIM] xtnd_table_init failed rc=%d\n", (int)irc);
        return 1;
    }

    xtnd_fn_t fn_print = (xtnd_fn_t)xtnd_find(&tab, "xtnd_print");
    if (!fn_print) {
        printf("[SIM] Failed to resolve plugin function xtnd_print=%p\n", (void*)fn_print);
        return 1;
    }

    int rc_print = xtnd_call(&tab, (void*)fn_print, 4, 5);

    if (rc_print == 9)
        printstrln("PASS");
    else
        printintln(rc_print);

    return 0;
}
