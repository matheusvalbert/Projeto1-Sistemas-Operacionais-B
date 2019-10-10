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

	close(fd);
	for(i=0;i<16;i++)
	printf("The enviada message is: [%d]\n", receive[i]);
	strcpy(stringToSend, "d");
	strcat(stringToSend,receive);
/*	stringToSend[0] = 'd';
	stringToSend[1] = 11;
	stringToSend[2] = 155;
	stringToSend[3] = 21;
	stringToSend[4] = 218;
	stringToSend[5] = 75;
	stringToSend[6] = 68;
	stringToSend[7] = 160;
	stringToSend[8] = 245;
	stringToSend[9] = 21;
	stringToSend[10] = 29;
	stringToSend[11] = 207;
	stringToSend[12] = 196;
	stringToSend[13] = 192;
	stringToSend[14] = 31;
	stringToSend[15] = 53;
	stringToSend[16] = 213;
	stringToSend[17] = '\0';*/



	/*stringToSend[0] = 'd';
  	for(i = 0; receive[i]; i++)
		stringToSend[i+1] = receive[i];
	stringToSend[i+1] = '\0';*/
	//printf("%s\n", stringToSend);
/*	sleep(2);	

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
	close(fd);*/
	close(fd);
   return 0;
}
























