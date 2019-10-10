#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <stdio_ext.h>
 
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

 
int hex_to_int(char c){
        int first = c / 16 - 3;
        int second = c % 16;
        int result = first*10 + second;
        if(result > 9) result--;
        return result;
}

int hex_to_ascii(char c, char d){
        int high = hex_to_int(c) * 16;
        int low = hex_to_int(d);
        return high+low;
}

int main(){
	int ret, fd, i, len, op;
	char buf = 0;
	char stringReceived[33];
   	char stringToSend[BUFFER_LENGTH];
	char word[33], outword[67];//33:32+1, 67:33*2+1
	
	do
	{
		printf("Choosse String or Hexa:\n");
		printf("1	-	Hexa\n");
		printf("2	-	Char\n");
		scanf("%d",&op);

	}while(op!=1 && op!=2);
	__fpurge(stdin);
	if(op==1)
	{
		printf("Hexa:\n");
		fgets(stringReceived, sizeof(stringReceived), stdin);
		strcpy(stringToSend, stringReceived);
	}
	else if(op==2)
	{
		printf("String:\n");
		fgets(word, sizeof(word), stdin);
    		len = strlen(word);
		if(word[len-1]=='\n')
        		word[--len] = '\0';

    		for(i = 0; i<len; i++)
		{
        		sprintf(outword+i*2, "%02X", word[i]);
    		}
    		printf("String to Hexa:%s\n", outword);
		strcpy(stringToSend, outword);
	}

   	fd = open("/dev/crypto", O_RDWR);             // Open the device with read/write access
  	if (fd < 0)
	{
     		perror("Failed to open the device...");
   		return errno;
  	}

  	ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   	if (ret < 0)
	{
      		perror("Failed to write the message to the device.");
      		return errno;
   	}

  	printf("Writing message to the device [%s].\n", stringToSend);

	printf("Press ENTER to read back from the device...\n");
   	getchar();

  	ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
  	if (ret < 0)
	{
     		perror("Failed to read the message from the device.");
     		return errno;
  	}
  	printf("The received message is: [%s]\n", receive);

	if(op==2)
	{	
		printf("Original string: ");
		int length = strlen(receive);
		for(i = 0; i < length; i++)
		{
	                if(i % 2 != 0)
			{
                        	printf("%c", hex_to_ascii(buf, receive[i]));
                	}
			else
			{
                        	buf = receive[i];
                	}
        	}
	}
	printf("\n");
  	printf("End of the program\n");
  	return 0;
}
