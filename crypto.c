#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/crypto.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <crypto/skcipher.h>
#include <linux/kernel.h>
#include <linux/stat.h>


#define PFX "cryptoapi-demo: "

MODULE_AUTHOR("Grupo SO");
MODULE_DESCRIPTION("Simple CryptoAPI");
MODULE_LICENSE("GPL");

/* ====== CryptoAPI ====== */

#define DATA_SIZE       16

static char *key = "";//Para receber parâmetros
static char *iv = "";

module_param(key, charp, 0000);
MODULE_PARM_DESC(key, "A string");
module_param(iv, charp, 0000);
MODULE_PARM_DESC(iv, "A string");

static void
hexdump(unsigned char *buf, unsigned int len)
{
        while (len--)
	{
	        printk("%d", *buf++);
	}

        printk("\n");
}

static void
cryptoapi_demo(void)
{
        struct crypto_skcipher *tfm = NULL;
	struct scatterlist sg[2];
	struct skcipher_request *req;
        int ret;
        char *input, *output;

        tfm = crypto_alloc_skcipher ("cbc(aes)", 0, 0);

        if (IS_ERR(tfm)) {
                printk("could nor allocate skcipher tfm\n");
                return;
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

	
        memset(input,0, DATA_SIZE);


	input[0] = 220;
	input[1] = 27;
	input[2] = 182;
	input[3] = 72;
	input[4] = 85;
	input[5] = 119;
	input[6] = 248;
	input[7] = 74;
	input[8] = 33;
	input[9] = 19;
	input[10] = 212;
	input[11] = 201;
	input[12] = 101;
	input[13] = 223;
	input[14] = 40;
	input[15] = 60;


	sg_init_one(&sg[0], input, DATA_SIZE);
	sg_init_one(&sg[1], output, DATA_SIZE);


	//printk(KERN_ERR PFX "IN B4 : "); hexdump(input, 16);
	//printk(KERN_ERR PFX "OUT B4: "); hexdump(encrypted, 16);//data_size ultimo parametro



	skcipher_request_set_crypt(req, &sg[0], &sg[1], 16, iv);
	crypto_skcipher_decrypt(req);
	crypto_skcipher_encrypt(req);
	
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
}

/* ====== Module init/exit ====== */

static int __init
init_cryptoapi_demo(void)
{
        cryptoapi_demo();

        return 0;
}

static void __exit
exit_cryptoapi_demo(void)
{
}

module_init(init_cryptoapi_demo);
module_exit(exit_cryptoapi_demo);



