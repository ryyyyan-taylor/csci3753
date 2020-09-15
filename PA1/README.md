# PA1

## File Specifics
-------------------
### csci3753_add.c
**location:** `/home/kernel/linux-hwe-4.15.0/`
- Defines the function which is actually called when the system call is executed.

### Makefile
**location**: `./arch/x86/kernel/`
- Ensures that come time to compile the kernel all necessary files are compiled. 

### syscall_64.tbl
**location**: `./arch/x86/syscalls/`
- Syscall table maps each system call to a specific number, which are how the system calls are referenced when called.

### syscalls.h
**location**: `./include/linux/`
- Just like any other header file, this defines the prototype of our implemented function and other syscalls.

## Running testCalls.c
`gcc testCalls.c && ./a.out`
This will compile and run testCalls.c. If working, both calls should return 0. The system log should also display the hello world message, the numbers passed to be added, as well as the resulting sum.
