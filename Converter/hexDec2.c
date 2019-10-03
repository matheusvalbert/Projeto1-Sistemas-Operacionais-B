
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
		strcpy(&str[i*4],"0000"); 
            break; 
        case '1': 
		strcpy(&str[i*4],"0001"); 
            break; 
        case '2': 
		strcpy(&str[i*4],"0010");
            break; 
        case '3': 
		strcpy(&str[i*4],"0011");
            break; 
        case '4': 
		strcpy(&str[i*4],"0100"); 
            break; 
        case '5': 
		strcpy(&str[i*4],"0101");
            break; 
        case '6': 
		strcpy(&str[i*4],"0110"); 
            break; 
        case '7': 
		strcpy(&str[i*4],"0111"); 
            break; 
        case '8': 
		strcpy(&str[i*4],"1000");
            break; 
        case '9': 
		strcpy(&str[i*4],"1001"); 
            break; 
        case 'A': 
        case 'a': 
		strcpy(&str[i*4],"1010"); 
            break; 
        case 'B': 
        case 'b': 
		strcpy(&str[i*4],"1011");
            break; 
        case 'C': 
        case 'c': 
		strcpy(&str[i*4],"1100");
            break; 
        case 'D': 
        case 'd':
		strcpy(&str[i*4],"1101"); 
            break; 
        case 'E': 
        case 'e':
		strcpy(&str[i*4],"1110");
            break; 
        case 'F': 
        case 'f': 
		strcpy(&str[i*4],"1111"); 
            break; 
        default: 
            printf("\nInvalid hexadecimal digit %c", 
                   hexdec[i]); 
        } 
        i++; 
    } 
}

/*BinToHex(char *bin[], char str []){
  
    long long int i = 0;

  
    while (bin[i]) { 
  
        switch (bin[i*4]) {
	case '0': 
		strcpy(&str[i*4],"0000"); 
            break; 
        case '1': 
		strcpy(&str[i*4],"0001"); 
            break; 
        case '2': 
		strcpy(&str[i*4],"0010");
            break; 
        case '3': 
		strcpy(&str[i*4],"0011");
            break; 
        case '4': 
		strcpy(&str[i*4],"0100"); 
            break; 
        case '5': 
		strcpy(&str[i*4],"0101");
            break; 
        case '6': 
		strcpy(&str[i*4],"0110"); 
            break; 
        case '7': 
		strcpy(&str[i*4],"0111"); 
            break; 
        case '8': 
		strcpy(&str[i*4],"1000");
            break; 
        case '9': 
		strcpy(&str[i*4],"1001"); 
            break; 
        case 'A': 
        case 'a': 
		strcpy(&str[i*4],"1010"); 
            break; 
        case 'B': 
        case 'b': 
		strcpy(&str[i*4],"1011");
            break; 
        case 'C': 
        case 'c': 
		strcpy(&str[i*4],"1100");
            break; 
        case 'D': 
        case 'd':
		strcpy(&str[i*4],"1101"); 
            break; 
        case 'E': 
        case 'e':
		strcpy(&str[i*4],"1110");
            break; 
        case 'F': 
        case 'f': 
		strcpy(&str[i*4],"1111"); 
            break; 
        default: 
            printf("\nInvalid hexadecimal digit %c", 
                   bin[i]); 
        } 
        i++; 
    } 
}
*/
// driver code 
int main() 
{ 
  
	
	char 	hexdec[17];
	char	bin[128];
	char 	hex[17]
	
	// Get the Hexadecimal number 
	scanf("%s", hexdec);
	// Convert HexaDecimal to Binary
	HexToBin(hexdec, bin);
 
	printf("PRINT: %s\n", str); 

	BinToHex(bin, hex);
} 

