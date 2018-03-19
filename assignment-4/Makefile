obj-m += 4_1.o
obj-m += 4_2.o
obj-m += 4_3.o
obj-m += 4_4.o
obj-m += 4_b.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
    
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
