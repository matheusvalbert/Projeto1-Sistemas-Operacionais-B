#include <crypto/hash.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <crypto/rng.h>
#include <crypto/md5.h>
#include <crypto/sha.h>
#include <crypto/internal/skcipher.h>
#include <linux/jiffies.h>
#include <linux/mutex.h> 

MODULE_AUTHOR("Projeto1 SO_B");
MODULE_DESCRIPTION("crypto api");
MODULE_LICENSE("GPL");
#define  DEVICE_NAME "crypto"
#define  CLASS_NAME  "cryptoapi"

#define DATA_SIZE       16

//Links para o device na area do usuario
static DEFINE_MUTEX(crypto_mutex);
static int    majorNumber;              
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static struct class*  cryptoClass  = NULL;
static struct device* cryptoDevice = NULL;


static char   msg[256] = {0};
static int tamanhomsg = 0;

//Receber parametros na hora da instalacao
static char *key_getu = "";
static char *iv_get = "";

module_param(key_getu, charp, 0000);
MODULE_PARM_DESC(key_getu, "00000000000000000000");
module_param(iv_get, charp, 0000);
MODULE_PARM_DESC(iv_get, "00000000000000000000");

//Links para o device na area do usuario
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

//Structs para criptografia e descriptografia
struct sdesc {
    struct shash_desc shash;
    char ctx[];
};

struct tcrypt_result {
	struct completion completion;
	int err;
};

//Conversao para hexa, apos receber os dados da area do usuario
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

//Callback
static void test_skcipher_cb(struct crypto_async_request *req, int error) {

	struct tcrypt_result *result = req->data;

	if (error == -EINPROGRESS)
		return;
	result->err = error;
	complete(&result->completion);
}

//Impressão em hexa
static void hexdump(unsigned char *buf, unsigned int len) {

        while (len--)
	{
		printk("%02x", *buf++);
	}

        printk("\n");
}

//Criptografia/descriptografia
static int cipher(int way,int numop,char ms[]) {

	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	struct tcrypt_result result;
	char key[20], iv[20], *input = NULL;
	int i = 0, ret;

	//Copia da key e iv
	while(i != 16) {
	
		key[i] = key_getu[i];
		if(key[i]=='\0')
		{
			key[i]='0';
		}
		iv[i] = iv_get[i];
		if(iv[i]=='\0')
		{
			iv[i]='0';
		}
		i++;
	}
	key[16]='\0';
	iv[16]='\0';

	//Escolha do algoritmo de criptografia
	tfm = crypto_alloc_skcipher("cbc(aes)", 0, 0);
	if (IS_ERR(tfm)) {

		ret = PTR_ERR(tfm);
		return ret;
	}

	//Criacao do request
	req = skcipher_request_alloc(tfm, GFP_KERNEL);
	if (IS_ERR(req)) {

		pr_err("ERROR: skcipher_request_alloc\n");
		ret = PTR_ERR(req);
		goto error_tfm;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG, test_skcipher_cb, &result);

	//Set da key
	ret = crypto_skcipher_setkey(tfm, key, DATA_SIZE);
	if (ret != 0) {

		pr_err("ERROR: crypto_skcipher_setkey\n");
		goto error_req;
	}

	//Alocação do dado que vai ser criptado/decriptado
	input = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!input) {

                printk("kmalloc(input) failed\n");
                goto out;
        }

	//Copia do dado para o input
	i=0;
	while(i!=16) {

		input[i] = ms[i+16*numop];
		i++;
	}

	//Link da scatterlist com o input dos dados e criacao do request para criptografia/descriptografia
	sg_init_one(&sg, input, DATA_SIZE);
	skcipher_request_set_crypt(req, &sg, &sg, DATA_SIZE, iv);
	init_completion(&result.completion);

	//Escolha do que vai ser feito criptografar/descriptografar
	if (way == 0) {

		printk("input criptografia: "); hexdump(input, DATA_SIZE);

		ret = crypto_skcipher_encrypt(req);

		printk("output criptografia: "); hexdump(input, DATA_SIZE);
	}
	else if(way == 1) {

		printk("input descriptografia: "); hexdump(input, DATA_SIZE);

		ret = crypto_skcipher_decrypt(req);

		printk("output descriptografia: "); hexdump(input, DATA_SIZE);
	}
	else {

		goto out;
	}

	//Verica e realiza a espera do fim da criptografia/descriptografia
	switch (ret) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		ret = wait_for_completion_interruptible(&result.completion);
		break;
	}

	i=0;
	while(i!=16) {

		msg[i+16*numop] = input[i];
		if(input[i] == 0) {

			tamanhomsg = i+16*numop;
		}
		else {
			tamanhomsg = i+16*numop+1;
		}
		i++;
	}

//Libera os espacos de memoria
error_req:
	skcipher_request_free(req);
error_tfm:
	crypto_free_skcipher(tfm);
	return ret;
out:
	kfree(input);
	key[0]='\0';
	iv[0]='\0';
	return 0;
}

static int hash(char input[]) {

	struct crypto_shash *alg;
	struct sdesc *sdesc; 
	char digest[60];
	int ret, size;
	
	//Escolha do algoritmo no hash
	alg = crypto_alloc_shash("sha1", 0, 0);
	if (IS_ERR(alg)) {
		return PTR_ERR(alg);
	}

	//Coleta o tamanho e aloca espaco para a hash
	size = sizeof(struct shash_desc) + crypto_shash_descsize(alg);
	sdesc = kmalloc(size, GFP_KERNEL);
	sdesc->shash.tfm = alg;
	if (IS_ERR(sdesc)) {
		pr_info("can't alloc sdesc\n");
		return PTR_ERR(sdesc);
	}

	//Realiza a criacao do hash e imprime os resultados
	ret = crypto_shash_digest(&sdesc->shash, input, tamanhomsg, digest);
	tamanhomsg = 20;
	printk("SHA1: "); hexdump(digest, 20);
	strcpy(msg,digest);
	kfree(sdesc);
	crypto_free_shash(alg);
	return ret;
}

//Abertura do device e lock no mutex
static int dev_open(struct inode *inodep, struct file *filep) {
	
	if(!mutex_trylock(&crypto_mutex)){

      		printk(KERN_ALERT "CryptoAPI: Device in use by another process");
      		return -EBUSY;
   	}
	return 0;
}

//Leitura da area do usuario
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
	
	int error_count = 0;

	error_count = copy_to_user(buffer,msg, tamanhomsg);

	if (error_count==0) {            
		return 1;
	}
	else {
		printk("Falhou no dev_read");
		return -EFAULT;             
	}
}

//Realizacao dos requests feitos pela area do usuario
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
	
	int i = 0, j=0, numop = 0;
	char *input = NULL;

	sprintf(msg, "%s", buffer); 	
	if(msg[0] == 'c') {

		//Retira o c do inicio da string
		while(msg[i+1]!='\0') {

			msg[i]=msg[i+1];
			i++;
		}
		msg[i] = '\0';
		tamanhomsg = strlen(msg);

		//Aloca o espaco para o input
		input = kmalloc(128, GFP_KERNEL);
		if (!input) {
		
		        printk("kmalloc(input) failed\n");
			goto outc;
		}

		i=0;
		//Transforma a string em numero hexa
		while(msg[i] != '\0') {
			
			msg[i] = convert(msg[i]);
			i++;
		}

		//Uniao dos numeros em hexa
		i=0;
		while(i != tamanhomsg/2) {

			input[i] = (msg[j]<<4) + msg[j+1];		
			
			i++;
			j+=2;
		}
		
		//Completa com 0 os espacos vazios
		while(i % 16 != 0) {
			
			input[i] = 0x00;	
			i++;
		}
		input[i] = '\0';
		tamanhomsg = i;
		
		numop = tamanhomsg/16;
		
		//Inicia criptografia
		i = 0;
		while(i!=numop) {

			cipher(0,i,input);
			i++;
		}		

outc:		kfree(input);

	}        
	else if(msg[0] == 'd') {

		//Retira o d do inicio da string
		while(msg[i+1]!='\0') {

			msg[i]=msg[i+1];
			i++;
		}
		msg[i] = '\0';
		tamanhomsg = strlen(msg);

		//Aloca a memoria para o input
		input = kmalloc(128, GFP_KERNEL);
		if (!input) {

		        printk("kmalloc(input) failed\n");
			goto outd;
		}

		//Transforma a string em numero hexa
		i=0;
		while(msg[i] != '\0') {
			
			msg[i] = convert(msg[i]);
			i++;
		}

		//Uniao dos numeros em hexa
		i=0;
		while(i != tamanhomsg/2) {

			input[i] = (msg[j]<<4) + msg[j+1];		
			
			i++;
			j+=2;
		}
		//Completa com 0 os espacos vazios
		while(i % 16 != 0) {
			
			input[i] = 0x00;	
			i++;
		}
		input[i] = '\0';
		tamanhomsg = i;

		//Inicia a descriptografia
		numop = tamanhomsg/16;
		i = 0;
		while(i!=numop) {

			cipher(1,i,input);
			i++;
		}
outd:		kfree(input);
	}   
	else if(msg[0] == 'h') {

		//Retira o h do inicio da string
		while(msg[i+1]!='\0') {

			msg[i]=msg[i+1];
			i++;
		}
		msg[i] = '\0';
		tamanhomsg = strlen(msg);

		input = kmalloc(128, GFP_KERNEL);
		if (!input) {

		        printk("kmalloc(input) failed\n");
			goto outd;
		}
		//Transforma a string em numero hexa
		i=0;
		while(msg[i] != '\0') {
			
			msg[i] = convert(msg[i]);
			i++;
		}

		//Uniao dos numeros em hexa
		i = 0;
		j = 0;
		while(i != tamanhomsg/2) {

			input[i] = (msg[j]<<4) + msg[j+1];		
			
			i++;
			j+=2;
		}
		//Inicia a o hash
		input[i] = '\0';
		tamanhomsg = strlen(input);
		hash(input);
	} 
	return 1;
	
}

//Abertura do mutex
static int dev_release(struct inode *inodep, struct file *filep) {
	
	mutex_unlock(&crypto_mutex);
	return 0;
}

// Inicializacao do modulo e mutex
static int __init cryptotest_init(void) {
	

	mutex_init(&crypto_mutex);
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0) {

		printk(KERN_ALERT "Crypto failed to register a major number\n");
		return majorNumber;
	}
	 
	cryptoClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(cryptoClass)) { 
             
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(cryptoClass);
	}
	 
	cryptoDevice = device_create(cryptoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(cryptoDevice)) {

		class_destroy(cryptoClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(cryptoDevice);
	}

	return 0;
}

//Finalizacao do modulo e destruicao do mutex
static void __exit cryptotest_exit(void) {

	device_destroy(cryptoClass, MKDEV(majorNumber, 0));
	class_unregister(cryptoClass);
	class_destroy(cryptoClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);  
	mutex_destroy(&crypto_mutex);
}

module_init(cryptotest_init);
module_exit(cryptotest_exit);
