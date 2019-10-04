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
cryptoapi_demo(void)
{
        struct crypto_skcipher *tfm = NULL;
	struct scatterlist sg[2];
	struct skcipher_request *req;
        int ret;
        char *input, *output;

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

        ret = crypto_skcipher_setkey(tfm, key, sizeof(key));

        if (ret) {
                printk(KERN_ERR PFX "error ret\n");
                goto out;
        }

        input = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!input) {
                printk(KERN_ERR PFX "kmalloc(input) failed\n");
		kfree(input);
                goto out;
        }

        output = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!output) {
                printk(KERN_ERR PFX "kmalloc(output) failed\n");
                kfree(output);
                goto out;
        }



	sg_init_one(&sg[0], input, DATA_SIZE);
	sg_init_one(&sg[1], output, DATA_SIZE);


	//printk(KERN_ERR PFX "IN B4 : "); hexdump(input, 16);
	//printk(KERN_ERR PFX "OUT B4: "); hexdump(encrypted, 16);//data_size ultimo parametro



	skcipher_request_set_crypt(req, &sg[0], &sg[1], 16, iv);

	
	//printk(KERN_ERR PFX "IN     AFTER: "); hexdump(input, 16);
        //printk(KERN_ERR PFX "OUTPUT AFTER: "); hexdump(output, 16);//data_size ultimo parametro

        if (memcmp(input, output, DATA_SIZE) != 0)
                printk(KERN_ERR PFX "FAIL: input buffer != decrypted buffer\n");
        else
                printk(KERN_ERR PFX "PASS: encryption/decryption verified\n");

        kfree(output);
        kfree(input);

out:
        crypto_free_skcipher(tfm);
	return 1;
}


static int dev_open(struct inode *inodep, struct file *filep){
	printk("Cripto opened");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
int error_count = 0;
// copy_to_user has the format ( * to, *from, size) and returns 0 on success
	//error_count = copy_to_user(buffer, , DATA_SIZE);
	if (error_count==0){            
		printk("Sent cripto\n");
		return 1;
	}
	else {
		printk("Falhou no dev_read");
		return -EFAULT;             
	}
}


static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	char message[256];
	sprintf(message, "%s(%zu letters)", buffer, len); 
	if(message[0] == 'c')
	{
		printk("crypt");
	}        
	if(message[0] == 'd')
	{
		printk("decrypt");
	}      
	return 1;
}


static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "close dev\n");
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


        cryptoapi_demo();

        return 0;
}

static void __exit
exit_cryptoapi_demo(void)
{
	device_destroy(cryptoClass, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(cryptoClass);                          // unregister the device class
	class_destroy(cryptoClass);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
}

module_init(init_cryptoapi_demo);
module_exit(exit_cryptoapi_demo);



