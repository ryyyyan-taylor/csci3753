# PA2

## Contact Info
Ryan Taylor
109290202
ryta4737@colorado.edu

## Files Created

### helloModule.c
- not included
- created per instruction in assignment writeup, test module to make sure LKMs working

### pa2_char_driver.c
- driver implementation including function defs for read, write, seek, init, exit, open, and close of the device

### Makefile
- used to compile c code into .ko (kernel objects) loadable modules

### /dev/simple_character_device
- actual device being read from and written to
- command to create device listed below

### driverTest.c
- userspace program created to test functionality of driver implementation

## Installation
1. `Makefile` and `pa2_char_driver.c` both go in `/home/kernel/modules directory`
2. run `make -C /lib/modules/$(uname -r)/build M=$PWD` in same directory to compile into loadable modules
3. `sudo insmod pa2_char_driver.ko` will insert the module into the kernel
4. `sudo mknod -m 777 /dev/simple_character_device c 122 0` as described in the writeup will add  the device into `/dev` directory and assign `122` as its major number
5. In any directory: `gcc driverTest.c && ./a.out` to start the test program
