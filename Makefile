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
