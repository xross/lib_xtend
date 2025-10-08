    .globl _start
    .globl xtnd_exports
    .align 4
_start:
xtnd_exports:
    .word 0x58544E44              /* 'XTND' */
    .word 1                       /* version = 1 (offset tables) */
    .word _cp - xtnd_exports     /* cp_off */
    .word _dp - xtnd_exports     /* dp_off */
    .word _CTOR_LIST_ - xtnd_exports
    .word _CTOR_END_ - xtnd_exports
    .word _DoSyscall - xtnd_exports
    .word __timers - xtnd_exports  /* timers_off */
    .word _fdp.bss - xtnd_exports  /* init_len */
     .word (_edp.bss.large - xtnd_exports)      /* mem_len (falls back if same) */
    .word 1                       /* count */

    /* fn_offs[1] */
    .word xtnd_global_add   - xtnd_exports

    /* name_offs[1] */
    .word name_xtnd_global_add   - xtnd_exports

    /* String pool */
name_xtnd_global_add:
    .asciz "xtnd_global_add"
    .align 4
