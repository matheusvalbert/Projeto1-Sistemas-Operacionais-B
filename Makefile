obj-m+=	crypto.o
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	 $(CC) cryptoteste.c -o test
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test


