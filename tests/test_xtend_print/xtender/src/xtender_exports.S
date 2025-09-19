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
    .word _fdp.bss - xtend_exports /* length */
    .word 1                       /* count */

    /* fn_offs[1] */
    .word xtend_print   - xtend_exports

    /* name_offs[1] */
    .word name_xtend_print   - xtend_exports

    /* String pool */
name_xtend_print:
    .asciz "xtend_print"
    .align 4
