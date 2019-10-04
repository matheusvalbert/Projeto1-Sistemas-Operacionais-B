#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>


#define DATA_SIZE       16             
 

int main(){
	char *input;
	int fd, ret;

	/*input = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!input) {
                printk(KERN_ERR PFX "kmalloc(input) failed\n");
		kfree(input);
                goto out;
        }*/
	printf("Digite a chave a ser usada:\n");

	fd = open("/dev/cryptoapi", O_RDWR);
	if (fd < 0){
      		perror("Falha ao abrir o drive...");
      		return errno;
   	}
	
	/*memset(input,0, DATA_SIZE);

	input[0] = 0x00;
	input[1] = 0x01;
	input[2] = 0x02;
	input[3] = 0x03;
	input[4] = 0x04;
	input[5] = 0x05;
	input[6] = 0x06;
	input[7] = 0x07;
	input[8] = 0x08;
	input[9] = 0x09;
	input[10] = 0x10;
	input[11] = 0x11;
	input[12] = 0x12;
	input[13] = 0x13;
	input[14] = 0x14;
	input[15] = 0x15;*/

	ret = write(fd, "blahh", strlen("blahh"));
	if (ret < 0){
		perror("Failed to write the message to the device.");
		return errno;
	}

	printf("Press ENTER to read back from the device...\n");
	getchar();
	 
	printf("Reading from the device...\n");
	//ret = read(fd, receive, BUFFER_LENGTH);        
	/*if (ret < 0){
		perror("Failed to read the message from the device.");
		return errno;
	}*/
	printf("End of the program\n");
	return 0;
}
