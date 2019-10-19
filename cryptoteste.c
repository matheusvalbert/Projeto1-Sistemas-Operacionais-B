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

char convert(char op) {
	switch(op) {

	case '0':
		return 0x0;
	case '1':
		return 0x1;
	case '2':
		return 0x2;
	case '3':
		return 0x3;
	case '4':
		return 0x4;
	case '5':
		return 0x5;
	case '6':
		return 0x6;
	case '7':
		return 0x7;
	case '8':
		return 0x8;
	case '9':
		return 0x9;
	case 'A':
	case 'a':
		return 0xa;
	case 'B':
	case 'b':
		return 0xb;
	case 'C':
	case 'c':
		return 0xc;
	case 'D':
	case 'd':
		return 0xd;
	case 'E':
	case 'e':
		return 0xe;
	case 'F':
	case 'f':
		return 0xf;
	}

	return 0;
}

void hexdump(unsigned char *buf, unsigned int len)
{
        while (len--)
	{
	        printf("%02X", *buf++);
	}

	printf("\n");
}

int main()
{
	int ret, fd, i=0,j=0, flag = 0, flag2 = 0;
	char op, op2;
	char stringToSend[BUFFER_LENGTH], stringReceive[BUFFER_LENGTH], stringaux[BUFFER_LENGTH];
	char buf;
	int tamanhostr,tamanhoaux;
	fd = open("/dev/crypto", O_RDWR);             // Abre o device
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
	
	do
	{
		printf("Choose option:\n");
		printf("	1 - String(apenas para criptografia e hash)\n");
		printf("	2 - Hexa\n");
		printf("	3 - Sair\n");
		__fpurge(stdin);
		scanf("%c",&op);
		
		switch(op)
		{
		case '1':
			printf("String: ");
			__fpurge(stdin);
	    		fgets(stringReceive, sizeof(stringReceive), stdin);

	    		tamanhostr = strlen(stringReceive);
	    		if(stringReceive[tamanhostr-1]=='\n')
				stringReceive[--tamanhostr] = '\0';
			stringToSend[0]=stringReceive[0];
			if(stringToSend[0] == 'c' || stringToSend[0] == 'h')
			{
			    	for(i = 0; i<tamanhostr-1; i++)
				{
					sprintf(stringToSend+i*2+1, "%02X", stringReceive[i+1]);
			    	}
				stringToSend[i*2+1]='\0';
			    	printf("String to send(hexa): %s\n", stringToSend);
			}
			else
				strcpy(stringToSend,stringReceive);
			printf("%s\n", stringToSend);
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
			tamanhostr = strlen(receive);
			switch(stringToSend[0])
			{
				case 'c':
					printf("\nMensagem criptografada em hexa: ");
					hexdump(receive,tamanhostr);
					break;
				
				case 'h':
					printf("Resultado do Hash: ");
					hexdump(receive,20);
					break;
				default:
					printf("Operacao invalida.\n");
					break;
			}
			break;
		case '2':
			printf("Hexa: ");
			__fpurge(stdin);
			fgets(stringToSend, sizeof(stringToSend), stdin);
			tamanhostr = strlen(stringToSend);
			printf("String to send(hexa): %s", stringToSend);

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
			tamanhostr = strlen(receive);			

			switch(stringToSend[0])
			{
				case 'c':
					printf("\nMensagem criptografada: ");
					hexdump(receive,tamanhostr);
					break;
				case 'd':
					do
					{
						printf("O resultado deve impresso em:\n");
						printf("1	- Hexa\n");
						printf("2	- ASCII\n");
						printf("3	- Retorno para o menu inicial\n");
						__fpurge(stdin);
						scanf("%c", &op2);
						switch(op2)
						{
							case '1':
								printf("\nMensagem descriptografada em hexa: ");
								hexdump(receive,tamanhostr);
							break;
							case '2':
								printf("\nMensagem descriptografada em ASCII: ");
								strcpy(stringaux,receive);
								tamanhoaux = strlen(stringaux);
								i=0;
								while(stringaux[i]!='\0') {
									printf("%c",stringaux[i]);
									i++;
								}
								printf("\n");
							break;
							case '3':
								flag2 = 1;
							break;
							default:
								printf("Opcao invalida\n");
							break;
						}
					}while(flag2 == 0);
					flag2 = 0;
					break;
				case 'h':
					printf("Resultado do Hash: ");
					hexdump(receive,20);
					break;
				default:
					printf("Operacao invalida.\n");
					break;
			}
			break;	
		case '3':
			flag = 1;
			break;
		default:
			printf("Opcao invalida.\n");
			break;
		}

	}while(flag == 0);
	close(fd);
   return 0;
}
