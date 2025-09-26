
/* Auto-initialize libc hardware lock for plugins lacking full CRT startup.
 * If the standard runtime already set __libc_hwlock this is a no-op.
 * Define XTND_NO_LIBC_LOCK_INIT to disable.
 */
#ifndef XTND_NO_LIBC_LOCK_INIT

extern unsigned __libc_hwlock;

void __attribute__((constructor)) xtnd_libc_lock_ctor(void)
{
    if(__libc_hwlock == 0)
    {
        unsigned l;
        asm("getr %0, 0x5" : "=r"(l));
        __libc_hwlock = l;
    }
}

#endif
