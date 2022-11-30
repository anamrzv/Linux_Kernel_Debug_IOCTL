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

build_user:
	gcc -c my_user_debug.c -l/usr/src/linux-headers-3.2.0-4-common/include/
