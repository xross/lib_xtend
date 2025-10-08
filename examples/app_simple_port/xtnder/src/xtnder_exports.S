    .globl _start
    .globl xtnd_exports
    .align 4
/* Export table:
 * magic, version(1), cp_off, dp_off, ctor_off, ctor_end_off, syscall_off, length, count,
 * fn_offs[count], name_offs[count], string pool (ASCIZ...), 4-byte aligned end.
 * All offsets are relative to xtnd_exports.
 */
_start:
xtnd_exports:
        .word 0x58544E44          /* 'XTND' */
    .word 1                       /* version = 1 (offset tables) */
    .word _cp - xtnd_exports     /* cp_off */
    .word _dp - xtnd_exports     /* dp_off */
    .word _CTOR_LIST_ - xtnd_exports
    .word _CTOR_END_ - xtnd_exports
    .word _DoSyscall - xtnd_exports
    .word __timers - xtnd_exports   /* timers_off */
    .word _fdp.bss - xtnd_exports /* init_len */
     .word (_edp.bss.large - xtnd_exports)      /* mem_len (falls back if same) */
    .word 1                       /* function count */

    /* fn_offs[1] */
    .word xtnd_port_toggle   - xtnd_exports

    /* name_offs[1] */
    .word name_xtnd_port_toggle   - xtnd_exports

    /* String pool */
name_xtnd_port_toggle:
    .asciz "xtnd_port_toggle"
    .align 4
