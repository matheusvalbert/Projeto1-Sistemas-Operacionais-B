#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <stdio_ext.h>
 
#define BUFFER_LENGTH 256              
static char receive[BUFFER_LENGTH];
    
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



void hexdump(unsigned char *buf, unsigned int len)
{
        while (len--)
	{
	        printf("%02x", *buf++);
	}

	printf("\n");
}

int main()
{
	int ret, fd, i=0, flag = 0, op;
	char stringToSend[BUFFER_LENGTH], stringReceive[BUFFER_LENGTH];
	char buf;
	int tamanhostr;
	fd = open("/dev/crypto", O_RDWR);             // Abre o device
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
	
	do
	{
		printf("Choose option:\n");
		printf("	1 - String\n");
		printf("	2 - Hexa\n");
		scanf("%d",&op);
		if(op==1 || op==2)
			flag=1;
		else
			flag=0;
		
	}while(flag==0);

	if(op==1)
	{
		printf("String:");
		__fpurge(stdin);
    		fgets(stringReceive, sizeof(stringReceive), stdin);

    		tamanhostr = strlen(stringReceive);
    		if(stringReceive[tamanhostr-1]=='\n')
        		stringReceive[--tamanhostr] = '\0';
		stringToSend[0]=stringReceive[0];
    		for(i = 0; i<tamanhostr-1; i++)
		{
        		sprintf(stringToSend+i*2+1, "%02X", stringReceive[i+1]);
    		}
    		printf("String to send(hexa): %s\n", stringToSend);
	}
	else if(op==2)
	{
		printf("Hexa:");
		__fpurge(stdin);
		fgets(stringToSend, sizeof(stringToSend), stdin);
		tamanhostr = strlen(stringToSend);
		printf("String to send(hexa): %s", stringToSend);
		for(i = 1; i<tamanhostr; i++)
		{
	                if(i % 2 != 1)
			{
                        	printf("%c", hex_to_ascii(buf, stringToSend[i]));
                	}
			else
			{
                        	buf = stringToSend[i];
                	}
        	}
		printf("\n");
	}
	else
	{
		return 0;
	}
	
	ret = write(fd, stringToSend, strlen(stringToSend)); //Escrever no device
	if (ret < 0){
		perror("Failed to write the message to the device.");
		return errno;
	}
 
	ret = read(fd, receive, BUFFER_LENGTH);        // Receber os dados do device
	if (ret < 0){
		perror("Failed to read the message from the device.");
		return errno;
	}
	tamanhostr = strlen(stringToSend)-1;
	printf("Mensagem criptada: ");
	for(i=0;i<tamanhostr;i++)
		printf("%02x", receive[i]);

	stringToSend[0] = 'd';//descriptografar o resultado da string criptografada acima
  	for(i = 0; receive[i]; i++)
		stringToSend[i+1] = receive[i];
	stringToSend[i+1] = '\0';


	ret = write(fd, stringToSend, strlen(stringToSend)); // Escrever no device
	if (ret < 0){
		perror("Failed to write the message to the device.");
		return errno;
	}
 
	ret = read(fd, receive, BUFFER_LENGTH);        // Receber os dados do device
	if (ret < 0){
		perror("Failed to read the message from the device.");
		return errno;
	}
	tamanhostr = strlen(stringToSend)-1;
	printf("\nMensagem decriptada: ");
	for(i=0;i<tamanhostr;i++)
		printf("%c\n", receive[i]);

	strcpy(stringToSend,"hteste");//Teste do Hashe
	ret = write(fd, stringToSend, strlen(stringToSend)); // Escrever no device
	if (ret < 0){
		perror("Failed to write the message to the device.");
		return errno;
	}
 
	ret = read(fd, receive, BUFFER_LENGTH);        // Receber os dados do device
	if (ret < 0){
		perror("Failed to read the message from the device.");
		return errno;
	}
	printf("Hash: ");
	hexdump(receive,20);
	/*for(i=0;i<20;i++)
		printf("[%2x]", receive[i]);*/
	close(fd);
   return 0;
}
























