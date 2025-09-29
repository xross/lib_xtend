// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#ifndef _XTEND_H_
#define _XTEND_H_

/* Offsets (in words) within xtend_table_t (used in assembly) */
#define XTEND_OFF_CP_BASE_WORDS     (6)
#define XTEND_OFF_DP_BASE_WORDS     (7)

#define XTEND_EXPORT_MAGIC          (0x58544E44u) /* "XTND" */
#define XTEND_EXPORT_VERSION        (1)

#ifndef XTND_CALL_STACK_WORDS
#define XTND_CALL_STACK_WORDS       (10000)
#endif

#include <xs1.h>

#ifndef __ASSEMBLER__

#ifdef __xtend_conf_h_exists__
#include "xtend_conf.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <print.h>
#include <string.h>

#include "quadflash.h"


/* Export table layout :
 *   magic, version(1), cp_off, dp_off, ctors_off, ctors_end_off, syscall_off, init_len, mem_len,
 *   count, fn_offs[count], name_offs[count], string pool (ASCIZ...), align4.
 *
 * Offsets are all relative to the start of the table (xtend_header_t base).
 */

typedef int (*xtend_fn_t)(unsigned, unsigned);

typedef struct xtend_header {
    uint32_t magic;
    uint32_t version;
    uint32_t cp_off;
    uint32_t dp_off;
    uint32_t ctors_off;
    uint32_t ctors_end_off;
    uint32_t syscall_off;
    uint32_t init_len;        /* Length of code/data stored in flash */
    uint32_t mem_len;         /* Complete length of memoery required */
    uint32_t count;           /* Exported function count */
} xtend_header_t;

typedef struct xtend_table {
    const xtend_header_t *hdr;
    const uint32_t *fn_offs;
    const uint32_t *name_offs;
    const char *strings;
    size_t strings_len;
    uint8_t *blob_base;
    const uint8_t *cp_base;    /* blob_base + hdr->cp_off */
    const uint8_t *dp_base;    /* blob_base + hdr->dp_off */
    const uint8_t *ctors_base; /* blob_base + hdr->ctors_off */
    uint32_t ctors_count;
} xtend_table_t;

/* Status codes for xtend_init parsing */
typedef enum xtend_status {
    XTEND_OK = 0,           /* Success */
    XTEND_ERR_INPUT = 1,    /* Null ptrs or length < header */
    XTEND_ERR_MAGIC = 2,    /* Bad magic value */
    XTEND_ERR_VERSION = 3,  /* Unsupported version */
    XTEND_ERR_COUNT = 4,    /* Count would overflow arrays region */
    XTEND_ERR_LAYOUT = 5,   /* Arrays exceed blob length */
    XTEND_ERR_CPDP_MIS = 6, /* cp_off/dp_off misaligned */
    XTEND_ERR_CPDP_OOB = 7, /* cp_off/dp_off out of bounds */
    XTEND_ERR_CTORS = 8     /* Error running ctors */
} xtend_status_t;

/* Read an xtender from flash
 * Returns 0 on success, else flash error code
 */
int xtend_read(fl_QSPIPorts spiPort, uint8_t *blob_space, size_t blob_space_size);

/* Parse & validate an export table blob. Returns XTEND_OK on success. */
xtend_status_t xtend_init(uint8_t *blob, xtend_table_t *t);

/* Locate a function by name. Returns pointer or NULL. */
void *xtend_find(const xtend_table_t *t, const char *name);

/* Call a plugin function */
int xtend_call(const xtend_table_t *t, void *fn, unsigned a0, unsigned a1);

_Static_assert(XTEND_OFF_CP_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtend_table, cp_base), "offset changed: cp_base");
_Static_assert(XTEND_OFF_DP_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtend_table, dp_base), "offset changed: dp_base");

#endif
#endif // _XTEND_H_
