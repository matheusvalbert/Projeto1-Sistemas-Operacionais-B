#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM
 
int main(){
	int ret, fd, i=0;
	char stringToSend[BUFFER_LENGTH];
	fd = open("/dev/crypto", O_RDWR);             // Open the device with read/write access
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
  
	strcpy(stringToSend,"c0123456789abcdef");
	ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
	if (ret < 0){
		perror("Failed to write the message to the device.");
		return errno;
	}
 
	ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
	if (ret < 0){
		perror("Failed to read the message from the device.");
		return errno;
	}
	for(i=0;i<16;i++)
	printf("The enviada message is: [%d]\n", receive[i]);

	stringToSend[0] = 'd';
  	for(i = 0; receive[i]; i++)
		stringToSend[i+1] = receive[i];
	stringToSend[i+1] = '\0';
	//printf("%s\n", stringToSend);


	ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
	if (ret < 0){
		perror("Failed to write the message to the device.");
		return errno;
	}
 
	ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
	if (ret < 0){
		perror("Failed to read the message from the device.");
		return errno;
	}
	for(i=0;i<16;i++)
	printf("The recebida message is: [%d]\n", receive[i]);
   return 0;
}
























