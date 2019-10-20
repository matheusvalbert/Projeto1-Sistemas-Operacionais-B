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

///////////////////////////////////////////////////////////////////////////
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

static char *key_getu = "";
static char *iv_get = "";

module_param(key_getu, charp, 0000);
MODULE_PARM_DESC(key_getu, "00000000000000000000");
module_param(iv_get, charp, 0000);
MODULE_PARM_DESC(iv_get, "00000000000000000000");

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

struct sdesc {
    struct shash_desc shash;
    char ctx[];
};

///////////////////////////////////////////////////////////////////////////

struct tcrypt_result {
	struct completion completion;
	int err;
};

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

static void test_skcipher_cb(struct crypto_async_request *req, int error)
{
	struct tcrypt_result *result = req->data;

	if (error == -EINPROGRESS)
		return;
	result->err = error;
	complete(&result->completion);
}

static void
hexdump(unsigned char *buf, unsigned int len)
{
        while (len--)
	{
	        printk("%02x", *buf++);
	}

        printk("\n");
}

static int cipher(int way,int numop,char ms[])
{
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	char key[20], iv[20];
	int i = 0;
	int ret;
	char *input = NULL;
	struct tcrypt_result result;
//////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////
	tfm = crypto_alloc_skcipher("cbc(aes)", 0, 0);
	if (IS_ERR(tfm)) {
		ret = PTR_ERR(tfm);
		return ret;
	}

	req = skcipher_request_alloc(tfm, GFP_KERNEL);
	if (IS_ERR(req)) {
		pr_err("ERROR: skcipher_request_alloc\n");
		ret = PTR_ERR(req);
		goto error_tfm;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG, test_skcipher_cb, &result);


	ret = crypto_skcipher_setkey(tfm, key, DATA_SIZE);
	if (ret != 0) {
		pr_err("ERROR: crypto_skcipher_setkey\n");
		goto error_req;
	}

	input = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!input) {
                printk("kmalloc(input) failed\n");
                goto out;
        }

	i=0;
	while(i!=16)
	{
		input[i] = ms[i+16*numop];
		i++;
	}
	i=0;

	sg_init_one(&sg, input, DATA_SIZE);

	skcipher_request_set_crypt(req, &sg, &sg, DATA_SIZE, iv);
	init_completion(&result.completion);

	if (way == 0) {

		printk("pre input: "); hexdump(input, DATA_SIZE);

		ret = crypto_skcipher_encrypt(req);

		printk("pos input: "); hexdump(input, DATA_SIZE);
	}
	else if(way == 1){

		printk("pre output: "); hexdump(input, DATA_SIZE);

		ret = crypto_skcipher_decrypt(req);

		printk("pos output: "); hexdump(input, DATA_SIZE);
	}
	else {

		goto out;
	}

	switch (ret) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		ret = wait_for_completion_interruptible(&result.completion);
		break;
	}

	while(i!=16)
	{
		msg[i+16*numop] = input[i];
		i++;
	}

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

////////////////////////////////////////////////////////////////////////////////

static int hash(void) {

	struct crypto_shash *alg;
	struct sdesc *sdesc; 
	char digest[60];
	int ret, size;
	alg = crypto_alloc_shash("sha1", 0, 0);
	if (IS_ERR(alg)) {
		return PTR_ERR(alg);
	}

	size = sizeof(struct shash_desc) + crypto_shash_descsize(alg);
	sdesc = kmalloc(size, GFP_KERNEL);
	
	sdesc->shash.tfm = alg;

	if (IS_ERR(sdesc)) {
		pr_info("can't alloc sdesc\n");
		return PTR_ERR(sdesc);
	}

	ret = crypto_shash_digest(&sdesc->shash, msg, tamanhomsg, digest);
	tamanhomsg = 20;
	printk("SHA1: "); hexdump(digest, 20);
	strcpy(msg,digest);
	kfree(sdesc);
	crypto_free_shash(alg);
	return ret;
}

//////////////////////////////////////////////////////////////////////////////// f04d5c594c0de21b11a194844305ff3e

static int dev_open(struct inode *inodep, struct file *filep){
	if(!mutex_trylock(&crypto_mutex)){    /// Try to acquire the mutex (i.e., put the lock on/down)
                                          /// returns 1 if successful and 0 if there is contention
      		printk(KERN_ALERT "CryptoAPI: Device in use by another process");
      		return -EBUSY;
   	}
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_count = 0;
	//int numop;
// copy_to_user has the format ( * to, *from, size) and returns 0 on success
	//printk("retorno");hexdump(msg, tamanhomsg);
	//numop = tamanhomsg/16;
	//tamanhomsg = (numop+1)*16 - tamanhomsg%16;
	error_count = copy_to_user(buffer,msg, tamanhomsg);
	if (error_count==0){            
		return 1;
	}
	else {
		printk("Falhou no dev_read");
		return -EFAULT;             
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	int i = 0, j=0;
	int numop = 0;
	char *input = NULL;

	sprintf(msg, "%s", buffer); 	
	if(msg[0] == 'c')
	{
		//printk("mensagem %s", msg);

		while(msg[i+1]!='\0')
		{
			msg[i]=msg[i+1];
			i++;
		}
		msg[i] = '\0';
		tamanhomsg = strlen(msg);

		input = kmalloc(128, GFP_KERNEL);
		if (!input) {
		        printk("kmalloc(input) failed\n");
			goto outc;
		}

		i=0;
		while(msg[i] != '\0') {
			
			msg[i] = convert(msg[i]);
			i++;
		}

		i=0;
		while(i != tamanhomsg/2) {

			input[i] = (msg[j]<<4) + msg[j+1];		
			
			i++;
			j+=2;
		}
		
		while(i % 16 != 0)
		{
			
			input[i] = 0x00;	
			i++;
		}
		input[i] = '\0';
		tamanhomsg = i;
		//////////////////AQUIIIIII///////////////////////
		//printk("teste %i", tamanhomsg); hexdump(input, 20);
		i = 0;
		numop = tamanhomsg/16;
		while(i!=numop)
		{
			//printk("cipher no %i %i", numop,tamanhomsg);
			cipher(0,i,input);
			i++;
		}		

outc:		kfree(input);

	}        
	else if(msg[0] == 'd')
	{
		while(msg[i+1]!='\0')
		{
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

		i=0;
		while(msg[i] != '\0') {
			
			msg[i] = convert(msg[i]);
			i++;
		}

		//printk("Ak"); hexdump(msg, 32);
		
		i=0;
		while(i != tamanhomsg/2) {

			input[i] = (msg[j]<<4) + msg[j+1];		
			
			i++;
			j+=2;
		}
		tamanhomsg = i;

		//printk("AQUI"); hexdump(input, 16);
		//////////////////AQUIIIIII///////////////////////
		//printk("teste %i", tamanhomsg); hexdump(input, 20);
		i = 0;
		numop = tamanhomsg/16;
		while(i!=numop)
		{
			//printk("cipher no %i %i", numop,tamanhomsg);
			cipher(1,i,input);
			i++;
		}
outd:		kfree(input);
	}   
	else if(msg[0] == 'h')
	{
		while(msg[i+1]!='\0')
		{
			msg[i]=msg[i+1];
			i++;
		}
		msg[i] = '\0';
		tamanhomsg = strlen(msg);
		hash();
	} 
	return 1;
	
}


static int dev_release(struct inode *inodep, struct file *filep){
	mutex_unlock(&crypto_mutex);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int __init cryptotest_init(void)
{
	
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0)
	{
		printk(KERN_ALERT "Crypto failed to register a major number\n");
	      return majorNumber;
	}
	 
	cryptoClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(cryptoClass))
	{              
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(cryptoClass);
	}
	 
	cryptoDevice = device_create(cryptoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(cryptoDevice))
	{               
		class_destroy(cryptoClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(cryptoDevice);
	}

	return 0;
}

static void __exit cryptotest_exit(void)
{
	device_destroy(cryptoClass, MKDEV(majorNumber, 0));
	class_unregister(cryptoClass);
	class_destroy(cryptoClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);  
}

module_init(cryptotest_init);
module_exit(cryptotest_exit);
