# Initial process that execs /init.
# This code runs in user space.

#include "syscall.h"

# exec(init, argv)
.globl start
start:
        la a0, init             # Places arguments for exec in registers a0 and a1.
        la a1, argv             
        li a7, SYS_exec         # Puts the system call number in a7.
        ecall                   # Traps into the kernel and causes uservec, usertrap, and then syscall.

# for(;;) exit();
exit:
        li a7, SYS_exit
        ecall
        jal exit

# char init[] = "/init\0";
init:
  .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
  .long init
  .long 0
