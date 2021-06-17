obj-m += escape.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

start:
	sudo dmesg -C
	sudo insmod escape.ko
	dmesg

stop:
	sudo dmesg -C
	sudo rmmod escape.ko
	dmesg


why smoke weed when i have meth
all brown folk should bleed to death
