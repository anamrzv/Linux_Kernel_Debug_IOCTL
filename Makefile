obj-m += my_kernel_debug.o
 
PWD := $(CURDIR)
 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	insmod ./my_kernel_debug.ko

unload:
	rmmod ./my_kernel_debug.ko

build1:
	gcc -pedantic-errors -Wall -Werror -g3 -O0 --std=c99 -c my_user_debug.c 

build2:
	gcc my_user_debug.o -o main
