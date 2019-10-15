#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256              
static char receive[BUFFER_LENGTH];    
 
int main(){
	int ret, fd, i=0;
	char stringToSend[BUFFER_LENGTH];
	int tamanhostr;
	fd = open("/dev/crypto", O_RDWR);             // Abre o device
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
  
	strcpy(stringToSend,"c0123456789abcdefabcdef0123456789");//Teste criptografia
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
	for(i=0;i<tamanhostr;i++)
		printf("Mensagem criptada: [%d]\n", receive[i]);

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
	for(i=0;i<tamanhostr;i++)
	printf("Mensagem decriptada: [%d]\n", receive[i]);

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
	printf("Hash:\n");
	for(i=0;i<20;i++)
		printf("[%hhx]", receive[i]);
	close(fd);
   return 0;
}
























