    .globl _start
    .globl xtend_exports
    .align 4
_start:
xtend_exports:
    .word 0x58544E44              /* 'XTND' */
    .word 1                       /* version = 1 (offset tables) */
    .word _cp - xtend_exports     /* cp_off */
    .word _dp - xtend_exports     /* dp_off */
    .word _CTOR_LIST_ - xtend_exports
    .word _CTOR_END_ - xtend_exports
    .word _DoSyscall - xtend_exports
    .word _fdp.bss - xtend_exports /* init_len */
     .word (_edp.bss.large - xtend_exports)      /* mem_len (falls back if same) */
    .word 2                       /* count */

    /* fn_offs[2] */
    .word xtend_add   - xtend_exports
    .word xtend_sub   - xtend_exports

    /* name_offs[2] */
    .word name_xtend_add   - xtend_exports
    .word name_xtend_sub   - xtend_exports

    /* String pool */
name_xtend_add:
    .asciz "xtend_add"
name_xtend_sub:
    .asciz "xtend_sub"
    .align 4
