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
#include <crypto/hash.h>

MODULE_LICENSE("GPL");

#define DATA_SIZE 16

struct sdesc {
    struct shash_desc shash;
    char ctx[];
};

static void hexdump(unsigned char *buf, unsigned int len) {

        while (len--) {

		printk("%02x", *buf++);
	}

        printk("\n");
}

static int hash(void) {

	struct crypto_shash *alg;
	struct sdesc *sdesc;
	char data[1] = {"a"};
	char digest[40];
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

	ret = crypto_shash_digest(&sdesc->shash, data, DATA_SIZE, digest);

	printk("SHA1: "); hexdump(digest, 20);
	
	kfree(sdesc);
	crypto_free_shash(alg);
	return ret;
}

static int __init
init_cryptoapi_demo(void)
{

	hash();
	
        return 0;
}

static void __exit
exit_cryptoapi_demo(void)
{
}

module_init(init_cryptoapi_demo);
module_exit(exit_cryptoapi_demo);
