#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/crypto.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <crypto/skcipher.h>

MODULE_AUTHOR("Crypto");
MODULE_DESCRIPTION("Simple CryptoAPI");
MODULE_LICENSE("GPL");

#define PFX "cryptoapi-demo: "
#define DATA_SIZE       16

static char *key = "";
static char *iv = "";
static char   message[256] = {0}; 

module_param(key, charp, 0000);
MODULE_PARM_DESC(key, "A character string");
module_param(iv, charp, 0000);
MODULE_PARM_DESC(iv, "A character string");

static void hexdump(unsigned char *buf, unsigned int len) {

        while (len--) {

		printk("%02x", *buf++);
	}

        printk("\n");
}

static void init_cryptoapi(void) {

	struct crypto_skcipher *tfm = NULL;
	struct scatterlist sg[2];
	struct skcipher_request *req;
        int ret;
        char *input, *output;
	int opcao;

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

        ret = crypto_skcipher_setkey(tfm, key, DATA_SIZE);
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

	input[0] = 0x00;
	input[1] = 0x01;
	input[2] = 0x02;
	input[3] = 0x03;
	input[4] = 0x04;
	input[5] = 0x05;
	input[6] = 0x06;
	input[7] = 0x07;
	input[8] = 0x08;
	input[9] = 0x09;
	input[10] = 0x10;
	input[11] = 0x11;
	input[12] = 0x12;
	input[13] = 0x13;
	input[14] = 0x14;
	input[15] = 0x15;

	sg_init_one(&sg[0], input, DATA_SIZE);
	sg_init_one(&sg[1], output, DATA_SIZE);

	skcipher_request_set_crypt(req, &sg[0], &sg[1], 16, iv);

	opcao = 0;

	if(opcao == 0) {

		crypto_skcipher_encrypt(req);
	}
	else if(opcao == 1) {
	
		crypto_skcipher_decrypt(req);
	}

	printk(KERN_ERR PFX "IN: "); hexdump(input, 16);
        printk(KERN_ERR PFX "OUT: "); hexdump(output, 16);

        kfree(input);
	kfree(output);

out:
	crypto_free_skcipher(tfm);
}

static int __init cryptoapi_init(void) {

	init_cryptoapi();

	return 0;
}

static void __exit cryptoapi_exit(void) {}

module_init(cryptoapi_init);
module_exit(cryptoapi_exit);
















































