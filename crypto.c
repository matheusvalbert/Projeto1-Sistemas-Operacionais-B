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
MODULE_PARM_DESC(key_getu, "A string");
module_param(iv_get, charp, 0000);
MODULE_PARM_DESC(iv_get, "A string");

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

//////////////////////////////////////////////////////////////////////////


struct tcrypt_result {
	struct completion completion;
	int err;
};

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

static int cipher(int way,int numop)
{
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	char key[16], iv[16];
	int i = 0;
	int ret;
	char *input = NULL;
	struct tcrypt_result result;
	
//////////////////////////////////////////////////////////////////////

	while(i != 16) {
	
		key[i] = key_getu[i];
		iv[i] = iv_get[i];
		i++;
	}

///////////////////////////////////////////////////////////////////////

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
		input[i] = msg[i+16*numop];
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

////////////////////////////////////////////////////////////////////////////////

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
// copy_to_user has the format ( * to, *from, size) and returns 0 on success
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
	int i = 0;
	int numop = 0;

	sprintf(msg, "%s", buffer); 	
	if(msg[0] == 'c')
	{
		while(msg[i+1]!='\0')
		{
			msg[i]=msg[i+1];
			i++;
		}
		msg[i] = '\0';
		tamanhomsg = strlen(msg);
		printk("TESTE");
		hexdump(msg,20);
		hexdump(msg,32);
		numop = tamanhomsg/16;
		i = 0;
		while(i!=numop)
		{
			cipher(0,i);
			i++;
		}
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
		numop = tamanhomsg/16;
		i = 0;
		while(i!=numop)
		{
			cipher(1,i);
			i++;
		}
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
