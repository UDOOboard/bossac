## UDOO BOSSAC
The BOSSAC binary is used to upload the compiled sketch to Arduino Due on UDOO 
quad / dual boards.

This version is patched to trigger erase/reset on the Arduino, sending an auth 
token and opcode on the control serial lines. For more details, see drivers/misc/udoo_ard.c 
on the UDOO Linux Kernel.

### Supported platforms
* Linux (tested on Ubuntu 15.04)
* Windows (tested on Windows 7, 10)
* UDOOBuntu 2
