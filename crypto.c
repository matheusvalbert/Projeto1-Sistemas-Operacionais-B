#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/crypto.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <crypto/skcipher.h>
#include <linux/kernel.h>
#include <linux/stat.h>   
#include <linux/device.h>       
#include <linux/fs.h>             
#include <linux/uaccess.h>   
#include <linux/mutex.h>    
#define  DEVICE_NAME "crypto"  
#define  CLASS_NAME  "cryptoapi"        

#define PFX "cryptoapi: "

MODULE_AUTHOR("Grupo SO");
MODULE_DESCRIPTION("Simple CryptoAPI");
MODULE_LICENSE("GPL");

/* ====== CryptoAPI ====== */

#define DATA_SIZE       16

static char *key = "";//Para receber parâmetros
static char *iv = "";
static int    majorNumber;              
static int     dev_open(struct inode *, struct file *);//Para receber as funções
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static struct class*  cryptoClass  = NULL; ///< The device-driver class struct pointer
static struct device* cryptoDevice = NULL; ///< The device-driver device struct pointer
static char msg[256], auxmsg[35];//17:16+1, 35:17*2+1
static int tamanhomsg = 0;
static DEFINE_MUTEX(crypto_mutex);

struct tcrypt_result {
    struct completion completion;
    int err;
};

static	struct crypto_skcipher *tfm = NULL;
static	struct scatterlist sg[2];
static	struct skcipher_request *req = NULL;
static  int ret;
static  char *input, *output;
static struct tcrypt_result result;

module_param(key, charp, 0000);
MODULE_PARM_DESC(key, "A string");
module_param(iv, charp, 0000);
MODULE_PARM_DESC(iv, "A string");

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

//--------------------------CharToHexa--------------------------
static void
CharToHexa(void)
{
	int i = 0;
	printk("String: %s\n", msg);		
	if(msg[tamanhomsg-1]=='\n')
        	msg[--tamanhomsg] = '\0';

    	while( i<tamanhomsg)
	{
        	sprintf(auxmsg+i*2, "%02X", msg[i]);
		i++;
    	}
    	printk("String to Hexa:%s\n", auxmsg);
	msg[0]='\0';
	strcpy(msg,auxmsg);
	auxmsg[0]='\0';
}
//--------------------------------------------------------------

//--------------------------HexaToChar--------------------------
static int
hex_to_int(char c)
{
        int first = c / 16 - 3;
        int second = c % 16;
        int result = first*10 + second;
        if(result > 9) result--;
        return result;
}

static int
hex_to_ascii(char c, char d)
{
        int high = hex_to_int(c) * 16;
        int low = hex_to_int(d);
        return high+low;
}

static void
HexaToChar(void)
{
	int i = 0, j = 0;
	char buf = 0;
	auxmsg[0]='\0';
	while(i < tamanhomsg)
	{
		if(i % 2 != 0)
		{
                       	auxmsg[j] = (char)hex_to_ascii(buf, msg[i]);
			j++;
               	}
		else
		{
                       buf = msg[i];
	      	}
		i++;
        }
	auxmsg[j]='\0';
	msg[0]='\0';
	strcpy(msg,auxmsg);
	printk("Original string: %s\n", msg);
	auxmsg[0]='\0';
}
//--------------------------------------------------------------

static void
hexdump(unsigned char *buf, unsigned int len)
{
        while (len--)
	{
	        printk("%d", *buf++);
	}

        printk("\n");
}

void callback_function(struct crypto_async_request *req, int error)
{

}

static void test_skcipher_cb(struct crypto_async_request *req, int error)
{
    struct tcrypt_result *result = req->data;

    if (error == -EINPROGRESS)
        return;
    result->err = error;
    complete(&result->completion);
    pr_info("Encryption finished successfully\n");
}


static int
cryptoapi_demo(char operacao)
{
	int i=0, rc = 0;
        tfm = crypto_alloc_skcipher ("cbc-aes", 0, 0);
	if (IS_ERR(tfm)) 
	{
		printk("could nor allocate skcipher tfm\n");
		return -1;
	}

	req = skcipher_request_alloc(tfm, GFP_KERNEL);
    	if (!req) {
        	printk("could not allocate tfm request\n");
        	goto out;
    	}

        ret = crypto_skcipher_setkey(tfm, key, strlen(key));

        if (ret) {
                printk(KERN_ERR PFX "error ret\n");
                goto out;
        }
	
   	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG, test_skcipher_cb, &result);

        input = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!input) {
                printk(KERN_ERR PFX "kmalloc(input) failed\n");
                goto out;
        }

        output = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!output) {
                printk(KERN_ERR PFX "kmalloc(output) failed\n");
                kfree(input);
                goto out;
        }
	while(i!=16)
	{
		input[i] = msg[i];
		i++;
	}
	i=0;
	sg_init_one(&sg[0], input, DATA_SIZE);
	sg_init_one(&sg[1], output, DATA_SIZE);
	skcipher_request_set_crypt(req, &sg[0], &sg[1], 16, iv);
	if(operacao == 'c')
	{
		crypto_skcipher_encrypt(req);
		printk(KERN_ERR PFX "input anterior: "); hexdump(input, 16);
		printk(KERN_ERR PFX "output anterior: "); hexdump(output, 16);
	}
	if(operacao == 'd')
	{
		
		crypto_skcipher_decrypt(req);
		printk("decrypted");
		printk(KERN_ERR PFX "output decriptada: "); hexdump(output, 16);
	}
	while(i!=16)
	{
		msg[i] = output[i];
		i++;
	}

	    switch (rc) {
		    case 0:
			break;
		    case -EINPROGRESS:
		    case -EBUSY:
			rc = wait_for_completion_interruptible(
			    &result.completion);
			if (!rc && !result.err) {
			    reinit_completion(&result.completion);
			    break;
			}
		    default:
			pr_info("skcipher encrypt returned with %d result %d\n",
			    rc, result.err);
			break;
    }
    init_completion(&result.completion);

        kfree(output);
        kfree(input);

out:
        crypto_free_skcipher(tfm);
	skcipher_request_free(req);
	return 1;
}


static int dev_open(struct inode *inodep, struct file *filep){
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

	tamanhomsg = strlen(msg);
	CharToHexa();	
	if(0){    /// Try to acquire the mutex (i.e., put the lock on/down)
                                          /// returns 1 if successful and 0 if there is contention
      		printk(KERN_ALERT "EBBChar: Device in use by another process");
      		return -EBUSY;
   	}
	else {
		sprintf(msg, "%s", buffer); 	
		if(msg[0] == 'c')
		{
			tamanhomsg = strlen(msg);
			while(i!=17)
			{
				msg[i]=msg[i+1];
				i++;
			}
			cryptoapi_demo('c');
		}        
		else if(msg[0] == 'd')
		{
		/*
			msg[0] = 'd';
			msg[1] = 0x41;
			msg[2] = 0x42;
			msg[3] = 0x43;
		*/
			cryptoapi_demo('d');
			//tamanhomsg = strlen(msg);
			HexaToChar();
		}      
		return 1;
	}
        return 0;
	
}


static int dev_release(struct inode *inodep, struct file *filep){
	mutex_unlock(&crypto_mutex);

	return 0;
}

/* ====== Module init/exit ====== */

static int __init
init_cryptoapi_demo(void)
{

	//Register major number
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0)
	{
		printk(KERN_ALERT "Crypto failed to register a major number\n");
	      return majorNumber;
	}
	 
	// Register the device class
	cryptoClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(cryptoClass)) // Check for error and clean up if there is
	{              
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(cryptoClass);          // Correct way to return an error on a pointer
	}
	 
	// Register the device driver
	cryptoDevice = device_create(cryptoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(cryptoDevice)) // Clean up if there is an error
	{               
		class_destroy(cryptoClass);           // Repeated code but the alternative is goto statements
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(cryptoDevice);
	}
	mutex_init(&crypto_mutex);

        return 0;
}

static void __exit
exit_cryptoapi_demo(void)
{
	device_destroy(cryptoClass, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(cryptoClass);                          // unregister the device class
	class_destroy(cryptoClass);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
	mutex_destroy(&crypto_mutex);
}

module_init(init_cryptoapi_demo);
module_exit(exit_cryptoapi_demo);



