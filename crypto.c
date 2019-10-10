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
static char   msg[256] = {0};
static int tamanhomsg = 0;
static DEFINE_MUTEX(crypto_mutex);


static	struct crypto_skcipher *tfm = NULL;
static	struct scatterlist sg[2];
static	struct skcipher_request *req = NULL;
static  int ret;
static  char *input, *output;

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



static void
hexdump(unsigned char *buf, unsigned int len)
{
        while (len--)
	{
	        printk("%d", *buf++);
	}

        printk("\n");
}

static int
cryptoapi_demo(char operacao)
{
	int i=0;
        tfm = crypto_alloc_skcipher ("cbc(aes)", 0, 0);
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
	
	if(!mutex_trylock(&crypto_mutex)){    /// Try to acquire the mutex (i.e., put the lock on/down)
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
			cryptoapi_demo('d');
		}        
		if(msg[0] == 'd')
		{
			msg[0] = 11;
			msg[1] = 155;
			msg[2] = 21;
			msg[3] = 218;
			msg[4] = 75;
			msg[5] = 68;
			msg[6] = 160;
			msg[7] = 245;
			msg[8] = 21;
			msg[9] = 29;
			msg[10] = 207;
			msg[11] = 196;
			msg[12] = 192;
			msg[13] = 31;
			msg[14] = 53;
			msg[15] = 213;
			/*while(i!=17)
			{
				msg[i]=msg[i+1];
				i++;
			}*/
			cryptoapi_demo('d');
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



