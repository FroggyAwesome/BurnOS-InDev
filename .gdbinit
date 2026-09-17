set architecture i386
set pagination off
target remote localhost:1234
file bin/kernel.bin
break main
