#include "xtnd.h"

.globl xtend_call
.type  xtend_call, @function

.align	4
.cc_top xtend_call.function,xtend_call

// TODO Dual issue and optimise
// Args: r0= tab, r1=fn, r2=a0, r3=a1
xtend_call:
    ENTSP_lu6     8
    stw r10, sp[3]
    stw r9, sp[4]

    // Save old CP, DP
    ldaw        r11, cp[0]           // r11 = old CP
    stw         r11, sp[1]           // save old CP
    ldaw        r11, dp[0]           // r10 = old DP
    stw         r11, sp[2]           // save old DP

    // Set new CP, DP
    ldc         r10, XTEND_OFF_CP_BASE_WORDS
    ldw         r11, r0[r10]
    set         cp, r11              // CP := new_cp

    ldc         r10, XTEND_OFF_DP_BASE_WORDS
    ldw         r11, r0[r10]
    set         dp, r11              // DP := new_dp

    // Check if a timer is allocated for this thread
    ldaw        r10, dp[__timers]
    get         r11, id
    ldw         r9, r10[r11]
    bt          r9, xtend_call.timer_ok
xtend_call.timer_bad:
    getr        r9, XS1_RES_TYPE_TIMER
    stw         r9, r10[r11]

xtend_call.timer_ok:
    mov         r11, r1
    mov         r0, r2
    mov         r1, r3

xtend_call.jump:
    bla         r11                   // Call fn

    // Anything with more than 4(?) returns wont work (unlikely..)
    // keep return value in r0

    // Restore caller CP, DP -----
    ldw         r11, sp[1]
    set         cp, r11
    ldw         r11, sp[2]
    set         dp, r11

xtend_call.return:
    ldw         r10, sp[3]
    ldw         r9, sp[4]
    retsp       8

    // TODO #define the stack words
    .cc_bottom xtend_call.function
	.set	xtend_call.nstackwords, XTND_CALL_STACK_WORDS
	.globl	xtend_call.nstackwords
	.set	xtend_call.maxcores,1
	.globl	xtend_call.maxcores
	.set	xtend_call.maxtimers,0
	.globl	xtend_call.maxtimers
	.set	xtend_call.maxchanends,0
	.globl	xtend_call.maxchanends


