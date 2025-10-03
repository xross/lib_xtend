#if 1

    .globl _start
    .globl xtend_exports
    .align 4
/* Export table:
 * magic, version(1), cp_off, dp_off, ctor_off, ctor_end_off, syscall_off, length, count,
 * fn_offs[count], name_offs[count], string pool (ASCIZ...), 4-byte aligned end.
 * All offsets are relative to xtend_exports.
 */
_start:
xtend_exports:
        .word 0x58544E44            /* 'XTND' */
    .word 1                         /* version = 1 (offset tables) */
    .word _cp - xtend_exports       /* cp_off */
    .word _dp - xtend_exports       /* dp_off */
    .word _CTOR_LIST_ - xtend_exports
    .word _CTOR_END_ - xtend_exports
    .word _DoSyscall - xtend_exports
    .word __timers - xtend_exports   /* timers_off */
    .word _fdp.bss - xtend_exports /* init_len */
    .word (_edp.bss.large - xtend_exports)      /* mem_len (falls back if same) */
    .word 2                         /* function count */

    /* fn_offs[2] */
    .word xtend_port_toggle_bit   - xtend_exports
    .word xtend_i2c_read   - xtend_exports

    /* name_offs[2] */
    .word name_xtend_port_toggle_bit   - xtend_exports
    .word name_xtend_i2c_read   - xtend_exports

    /* String pool */
name_xtend_port_toggle_bit:
    .asciz "xtend_port_toggle_bit"
name_xtend_i2c_read:
    .asciz "xtend_i2c_read"
    .align 4

#endif
