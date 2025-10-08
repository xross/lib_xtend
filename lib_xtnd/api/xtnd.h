// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#ifndef _XTND_H_
#define _XTND_H_

#ifdef __xtnd_conf_h_exists__
#include "xtnd_conf.h"
#endif

#include <xs1.h>

/* Offsets (in words) within xtnd_table_t (used in assembly)
 * These are word indices (uint32_t sized) from the start of xtnd_table on target (32-bit).
 * Update if struct layout changes. (Pointers and size_t assumed 4 bytes on target.)
 */
#define XTND_OFF_CP_BASE_WORDS     (6)
#define XTND_OFF_DP_BASE_WORDS     (7)
#define XTND_OFF_TIMERS_BASE_WORDS (10)

#define XTND_EXPORT_MAGIC          (0x58544E44u) /* "XTND" */
#define XTND_EXPORT_VERSION        (1)

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

typedef struct xtnd_header {
    uint32_t magic;
    uint32_t version;
    uint32_t cp_off;
    uint32_t dp_off;
    uint32_t ctors_off;
    uint32_t ctors_end_off;
    uint32_t syscall_off;
    uint32_t timers_off;      /* Offset to timers table (optional, 0 if none) */
    uint32_t init_len;        /* Length of code/data stored in flash */
    uint32_t mem_len;         /* Complete length of memory required */
    uint32_t fn_count;        /* Exported function count */
} xtnd_header_t;

typedef struct xtnd_table {
    const xtnd_header_t * UNSAFE hdr;
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
} xtnd_table_t;

/* Call a plugin function */
int xtnd_call(const xtnd_table_t * UNSAFE t, void * UNSAFE fn, unsigned a0, unsigned a1);

#if !defined(__XC__)

/* Export table layout :
 *   magic, version(1), cp_off, dp_off, ctors_off, ctors_end_off, syscall_off, timers_off,
 *   init_len, mem_len, fn_count, fn_offs[count], name_offs[count], string pool (ASCIZ...), align4.
 *
 * Offsets are all relative to the start of the table (xtnd_header_t base).
 * timers_off may be 0 if the plugin does not export a timers table.
 */

typedef int (*xtnd_fn_t)(unsigned, unsigned);

/* Status codes for xtnd_init parsing */
typedef enum xtnd_status {
    XTND_OK = 0,           /* Success */
    XTND_ERR_INPUT = 1,    /* Null ptrs or length < header */
    XTND_ERR_MAGIC = 2,    /* Bad magic value */
    XTND_ERR_VERSION = 3,  /* Unsupported version */
    XTND_ERR_COUNT = 4,    /* Count would overflow arrays region */
    XTND_ERR_LAYOUT = 5,   /* Arrays exceed blob length */
    XTND_ERR_CPDP_MIS = 6, /* cp_off/dp_off misaligned */
    XTND_ERR_CPDP_OOB = 7, /* cp_off/dp_off out of bounds */
    XTND_ERR_CTORS = 8,    /* Error running ctors */
    XTND_ERR_SIZE = 9,     /* Not enough space for blob */
} xtnd_status_t;

/* Read an xtnder from flash
 * Returns 0 on success, else flash error code
 */
int xtnd_read(fl_QSPIPorts spiPort, uint8_t *blob_space, size_t blob_space_size);

/* Parse & validate an export table blob. Returns XTND_OK on success. */
xtnd_status_t xtnd_init(uint8_t *blob, size_t blob_space_size, xtnd_table_t *t);

/* Locate a function by name. Returns pointer or NULL. */
void *xtnd_find(const xtnd_table_t *t, const char *name);

_Static_assert(XTND_OFF_CP_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtnd_table, cp_base), "offset changed: cp_base");
_Static_assert(XTND_OFF_DP_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtnd_table, dp_base), "offset changed: dp_base");
_Static_assert(XTND_OFF_TIMERS_BASE_WORDS * sizeof(uint32_t) == offsetof(struct xtnd_table, timers_base), "offset changed: timers_base");

#endif
#endif
#endif // _XTND_H_
