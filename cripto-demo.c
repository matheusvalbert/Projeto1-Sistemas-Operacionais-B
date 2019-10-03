

/* 
 * Simple demo explaining usage of the Linux kernel CryptoAPI.
 * By Michal Ludvig <michal@logix.cz>
 *    http://www.logix.cz/michal/
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/crypto.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <crypto/skcipher.h>

#define PFX "cryptoapi-demo: "

MODULE_AUTHOR("Michal Ludvig <michal@logix.cz>");
MODULE_DESCRIPTION("Simple CryptoAPI demo");
MODULE_LICENSE("GPL");

/* ====== CryptoAPI ====== */

#define DATA_SIZE       16

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
        /* config options */
        char key[16]={"0123456789ABCDEF"}, iv[16]={"0123456789ABCDEF"};

        /* local variables */
        struct crypto_skcipher *tfm = NULL;
	struct scatterlist sg[3];
	struct skcipher_request *req;
        int ret;
        char *input, *encrypted, *decrypted;

        //memset(key, 0, sizeof(key));
        //memset(iv, 0, sizeof(iv));
	//iv="0123456789ABCDEF";
	//key="0123456789ABCDEF";
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
                goto out;
        }

        encrypted = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!encrypted) {
                printk(KERN_ERR PFX "kmalloc(encrypted) failed\n");
                kfree(input);
                goto out;
        }

        decrypted = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!decrypted) {
                printk(KERN_ERR PFX "kmalloc(decrypted) failed\n");
                kfree(encrypted);
                kfree(input);
                goto out;
        }
	
        memset(input,0, DATA_SIZE);
	input[0] = 00;
	input[1] = 11;
	input[2] = 22;
	input[3] = 33;
	input[4] = 44;
	input[5] = 55;
	input[6] = 66;
	input[7] = 77;
	input[8] = 88;
	input[9] = 99;
	input[10] = 10;
	input[11] = 20;
	input[12] = 30;
	input[13] = 40;
	input[14] = 50;
	input[15] = 60;


	sg_init_one(&sg[0], input, DATA_SIZE);
	sg_init_one(&sg[1], encrypted, DATA_SIZE);
	sg_init_one(&sg[2], decrypted, DATA_SIZE);
	printk(KERN_ERR PFX "ANTES ENCRYPTED: "); hexdump(encrypted, 16);//data_size ultimo parametro
	printk(KERN_ERR PFX "ANTES DECRYPTED: "); hexdump(decrypted, 16);
	sg_copy_from_buffer(&sg[0],1,input, 16);
	skcipher_request_set_crypt(req, &sg[0], &sg[1], 16, iv);
	crypto_skcipher_encrypt(req);
	sg_copy_from_buffer(&sg[1],1,encrypted, 16);
	sg_copy_from_buffer(&sg[2],1,encrypted, 16);
	skcipher_request_set_crypt(req, &sg[1], &sg[2], 16, iv);
	crypto_skcipher_decrypt(req);
	
	printk(KERN_ERR PFX "IN: "); hexdump(input, 16);
        printk(KERN_ERR PFX "DPS ENCRYPTED: "); hexdump(encrypted, 16);//data_size ultimo parametro
	printk(KERN_ERR PFX "DPS DECRYPTED: "); hexdump(decrypted, 16);

        if (memcmp(input, decrypted, DATA_SIZE) != 0)
                printk(KERN_ERR PFX "FAIL: input buffer != decrypted buffer\n");
        else
                printk(KERN_ERR PFX "PASS: encryption/decryption verified\n");

        kfree(decrypted);
        kfree(encrypted);
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


