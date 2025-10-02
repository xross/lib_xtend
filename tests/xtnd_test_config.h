// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef XTND_TEST_CONFIG_H_
#define XTND_TEST_CONFIG_H_

/* ---- Allocation / Blob Settings ---- */
#ifndef XTND_TEST_BLOB_BSS_SLACK_BYTES
#define XTND_TEST_BLOB_BSS_SLACK_BYTES 128
#endif

/* Provided by generated blob header (xtend_blob_bin_len). */
#define XTND_TEST_BLOB_ALLOC_SIZE (xtend_blob_bin_len + XTND_TEST_BLOB_BSS_SLACK_BYTES)

/* ---- Simulator Defaults ---- */
#ifndef XTND_TEST_MAX_CYCLES
#define XTND_TEST_MAX_CYCLES 15000000
#endif

/* ---- Variant Handling (placeholder for future matrix refactor) ---- */
/* Example future usage:
 * #ifndef XTND_TEST_ENABLED_VARIANTS
 * #define XTND_TEST_ENABLED_VARIANTS "t0,t1"
 * #endif
 */

#endif /* XTND_TEST_CONFIG_H_ */
