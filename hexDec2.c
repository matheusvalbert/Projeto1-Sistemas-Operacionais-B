
// C program to convert 
// Hexadecimal number to Binary 
  
#include <stdio.h>
#include <string.h> 
  
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
  
// driver code 
int main() 
{ 
  
	// Get the Hexadecimal number 
	char 	hexdec[17]="4142430D4142430D";
	char	str[128];

	// Convert HexaDecimal to Binary 
	printf("\nEquivalent Binary value is : "); 
	HexToBin(hexdec, str); 
	printf("\nPRINT: %s\n", str); 
} 

