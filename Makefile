obj-m+=cryptoapi.o
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) inter.c -o inter
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm inter
