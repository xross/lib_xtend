// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#ifndef _XTEND_H_
#define _XTEND_H_

#ifdef __xtnd_conf_h_exists__
#include "xtnd_conf.h"
#endif

#include <xs1.h>

/* Offsets (in words) within xtend_table_t (used in assembly)
 * These are word indices (uint32_t sized) from the start of xtend_table on target (32-bit).
 * Update if struct layout changes. (Pointers and size_t assumed 4 bytes on target.)
 */
#define XTEND_OFF_CP_BASE_WORDS     (6)
#define XTEND_OFF_DP_BASE_WORDS     (7)
#define XTEND_OFF_TIMERS_BASE_WORDS (10)

#define XTEND_EXPORT_MAGIC          (0x58544E44u) /* "XTND" */
#define XTEND_EXPORT_VERSION        (1)

#ifndef XTND_CALL_STACK_WORDS
#define XTND_CALL_STACK_WORDS       (10000)
#endif

#ifdef __XC__
#define UNSAFE unsafe
#else
#define UNSAFE
#endif
#if !defined(__ASSEMBLER__)

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <print.h>
#include <string.h>

#include "quadflash.h"

typedef struct xtend_header {
    uint32_t magic;
    uint32_t version;
    uint32_t cp_off;
    uint32_t dp_off;
    uint32_t ctors_off;
    uint32_t ctors_end_off;
    uint32_t syscall_off;
    uint32_t timers_off;      /* Offset to timers table (optional, 0 if none) */
    uint32_t init_len;        /* Length of code/data stored in flash */
    uint32_t mem_len;         /* Complete length of memoery required */
    uint32_t fn_count;        /* Exported function count */
} xtend_header_t;

typedef struct xtend_table {
    const xtend_header_t * UNSAFE hdr;
    const uint32_t * UNSAFE fn_offs;
    const uint32_t * UNSAFE name_offs;
    const char * UNSAFE strings;
    size_t strings_len;
    uint8_t * UNSAFE blob_base;
    const uint8_t * UNSAFE cp_base;    /* blob_base + hdr->cp_off */
    const uint8_t * UNSAFE dp_base;    /* blob_base + hdr->dp_off */
    const uint8_t * UNSAFE ctors_base; /* blob_base + hdr->ctors_off */
    uint32_t ctors_count;
    const uint8_t * UNSAFE timers_base; /* blob_base + hdr->timers_off (optional) */
} xtend_table_t;

/* Call a plugin function */
int xtend_call(const xtend_table_t * UNSAFE t, void * UNSAFE fn, unsigned a0, unsigned a1);

#if !defined(__XC__)

/* Export table layout :
 *   magic, version(1), cp_off, dp_off, ctors_off, ctors_end_off, syscall_off, timers_off,
 *   init_len, mem_len, fn_count, fn_offs[count], name_offs[count], string pool (ASCIZ...), align4.
 *
 * Offsets are all relative to the start of the table (xtend_header_t base).
 * timers_off may be 0 if the plugin does not export a timers table.
 */

typedef int (*xtend_fn_t)(unsigned, unsigned);

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
    XTEND_ERR_CTORS = 8,    /* Error running ctors */
    XTEND_ERR_SIZE = 9,     /* Not enough space for blob */
} xtend_status_t;

/* Read an xtender from flash
 * Returns 0 on success, else flash error code
 */
int xtend_read(fl_QSPIPorts spiPort, uint8_t *blob_space, size_t blob_space_size);

/* Parse & validate an export table blob. Returns XTEND_OK on success. */
xtend_status_t xtend_init(uint8_t *blob, size_t blob_space_size, xtend_table_t *t);

/* Locate a function by name. Returns pointer or NULL. */
void *xtend_find(const xtend_table_t *t, const char *name);

_Static_assert(XTEND_OFF_CP_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtend_table, cp_base), "offset changed: cp_base");
_Static_assert(XTEND_OFF_DP_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtend_table, dp_base), "offset changed: dp_base");
_Static_assert(XTEND_OFF_TIMERS_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtend_table, timers_base), "offset changed: timers_base");

#endif
#endif
#endif // _XTEND_H_
