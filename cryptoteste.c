#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

// function to convert Hexadecimal to Binary Number 
void HexToBin(char *hexdec, char str []) 
{ 
  
    long long int i = 0; 
  
    while (hexdec[i]) { 
  
        switch (hexdec[i]) {
	case '0': 
		printf("0000");
		strcpy(&str[i*4],"0000"); 
            break; 
        case '1': 
            	printf("0001");
		strcpy(&str[i*4],"0001"); 
            break; 
        case '2': 
            	printf("0010"); 
		strcpy(&str[i*4],"0010");
            break; 
        case '3': 
           	printf("0011"); 
		strcpy(&str[i*4],"0011");
            break; 
        case '4': 
            	printf("0100");
		strcpy(&str[i*4],"0100"); 
            break; 
        case '5': 
            	printf("0101"); 
		strcpy(&str[i*4],"0101");
            break; 
        case '6': 
            	printf("0110");
		strcpy(&str[i*4],"0110"); 
            break; 
        case '7': 
            	printf("0111");
		strcpy(&str[i*4],"0111"); 
            break; 
        case '8': 
            	printf("1000"); 
		strcpy(&str[i*4],"1000");
            break; 
        case '9': 
            	printf("1001");
		strcpy(&str[i*4],"1001"); 
            break; 
        case 'A': 
        case 'a': 
            	printf("1010");
		strcpy(&str[i*4],"1010"); 
            break; 
        case 'B': 
        case 'b': 
            	printf("1011");
		strcpy(&str[i*4],"1011");
            break; 
        case 'C': 
        case 'c': 
            	printf("1100");
		strcpy(&str[i*4],"1100");
            break; 
        case 'D': 
        case 'd':
            	printf("1101");
		strcpy(&str[i*4],"1101"); 
            break; 
        case 'E': 
        case 'e':
            	printf("1110");
		strcpy(&str[i*4],"1110");
            break; 
        case 'F': 
        case 'f': 
            	printf("1111");
		strcpy(&str[i*4],"1111"); 
            break; 
        default: 
            printf("\nInvalid hexadecimal digit %c",  hexdec[i]); 
        } 
        i++; 
    } 
}

int main(){
   int ret, fd;
char stringReceived[33];
   char stringToSend[BUFFER_LENGTH];
   fd = open("/dev/crypto", O_RDWR);             // Open the device with read/write access
  if (fd < 0){
     perror("Failed to open the device...");
    return errno;
  }
printf("Type in a short string to send to the kernel module:\n");

  scanf("%[^\n]%*c", stringReceived);
HexToBin(stringReceived, stringToSend); 

  ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }

  printf("Writing message to the device [%s].\n", stringToSend);

printf("Press ENTER to read back from the device...\n");
   getchar();

  ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
  if (ret < 0){
     perror("Failed to read the message from the device.");
     return errno;
  }
  printf("The received message is: [%s]\n", receive);
  printf("End of the program\n");
  return 0;
}
