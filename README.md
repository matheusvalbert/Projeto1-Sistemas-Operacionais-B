# Introdução
```
Este projeto foi desenvolvido no sexto semestre na disciplina de Sistemas Operacionais B.
O seu objetivo foi criar módulo que realiza criptografia e descriptografia para Linux, no qual 
é utilizado o algoritmo cbc no modo aes. Sendo assim, necessário a utilização de uma chave e de
um vetor de inicialização para criptografia e descriptografia dos dados.
```
# Começando a utilizá-lo
```
Este projeto deve ser utilizado em um sistema operacional Linux.
Primeiramente após realizar o download dos arquivos deve-se utilizar o comando make
na pasta onde os arquivos se encontram.
Após isto, deve-se usar o comando:
"sudo insmod crypto.ko key_getu="0123456789ABCDEF" iv_get=""0123456789ABCDEF"".
A chave ou o vetor de inicialização podem ser alterados por qualquer valores.
```
 
