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

void BinToHex(char bindec[], char str []) 
{ 
    char num[4];
    long long int i = 0, y = 0; 
  
    while (bindec[i]) {
  	sprintf(num, "%c%c%c%c", bindec[i], bindec[i+1], bindec[i+2], bindec[i+3]);
	printf("%s\n",num);
        switch (atoi(num)) {
	case 0: 
		printf("0");
		str[y] = '0';
            break; 
        case 1: 
            	printf("1");
		str[y] = '1'; 
            break; 
        case 10: 
            	printf("2"); 
		str[y] = '2';
            break; 
        case 11: 
           	printf("3"); 
		str[y] = '3';
            break; 
        case 100: 
            	printf("4");
		str[y] = '4'; 
            break; 
        case 101: 
            	printf("5"); 
		str[y] = '5';
            break; 
        case 110: 
            	printf("6");
		str[y] = '6'; 
            break; 
        case 111: 
            	printf("7");
		str[y] = '7'; 
            break; 
        case 1000: 
            	printf("8"); 
		str[y] = '8';
            break; 
        case 1001: 
            	printf("9");
		str[y] = '9'; 
            break; 
        case 1010: 
            	printf("A");
		str[y] = 'A';
            break; 
        case 1011: 
            	printf("B");
		str[y] = 'B';
            break; 
        case 1100: 
            	printf("C");
		str[y] = 'C';
            break;  
        case 1101:
            	printf("D");
		str[y] = 'D';
            break;  
        case 1110:
            	printf("E");
		str[y] = 'E';
            break;  
        case 1111: 
            	printf("F");
		str[y] = 'F';
            break; 
        default: 
            printf("\nInvalid bin digit %s",  num); 
        } 
        i+=4; 
	y++;
    } 
	str[y] = '\0';
}

int main(){
   int ret, fd;
char stringReceived[33];
   char stringToSend[BUFFER_LENGTH], strReceived[BUFFER_LENGTH];
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

  printf("\nWriting message to the device [%s].\n", stringToSend);

printf("Press ENTER to read back from the device...\n");
   getchar();

  ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM

  if (ret < 0){
     perror("Failed to read the message from the device.");
     return errno;
  }
BinToHex (receive, stringReceived);
printf("\nThe received message is: [%s]\n", stringReceived);
  printf("End of the program\n");
  return 0;
}
