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

MODULE_AUTHOR("Projeto1 SO_B");
MODULE_DESCRIPTION("crypto api");
MODULE_LICENSE("GPL");

#define DATA_SIZE       16

static char *key_get = "";
static char *iv_get = "";

module_param(key_get, charp, 0000);
MODULE_PARM_DESC(key_get, "A string");
module_param(iv_get, charp, 0000);
MODULE_PARM_DESC(iv_get, "A string");

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
	        printk("%d", *buf++);
	}

        printk("\n");
}

static int do_test_cipher(int way)
{
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	char key[16], iv[16];
	int ret;
	char *input = NULL;
	struct tcrypt_result result;
	int i = 0;

	while(i != 16) {
	
		key[i] = key_get[i];
		iv[i] = iv_get[i];
		i++;
	}

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

	sg_init_one(&sg, input, DATA_SIZE);

	skcipher_request_set_crypt(req, &sg, &sg, DATA_SIZE, iv);
	init_completion(&result.completion);

	if (way == 0) {

		input[0] = 0;
		input[1] = 1;
		input[2] = 2;
		input[3] = 3;
		input[4] = 4;
		input[5] = 5;
		input[6] = 6;
		input[7] = 7;
		input[8] = 8;
		input[9] = 9;
		input[10] = 0xa;
		input[11] = 0xb;
		input[12] = 0xc;
		input[13] = 0xd;
		input[14] = 0xe;
		input[15] = 0xf;

		printk("pre input: "); hexdump(input, DATA_SIZE);

		ret = crypto_skcipher_encrypt(req);

		printk("pos input: "); hexdump(input, DATA_SIZE);
	}
	else {

		input[0] = 230;
		input[1] = 109;
		input[2] = 69;
		input[3] = 124;
		input[4] = 121;
		input[5] = 187;
		input[6] = 210;
		input[7] = 186;
		input[8] = 117;
		input[9] = 130;
		input[10] = 169;
		input[11] = 120;
		input[12] = 165;
		input[13] = 58;
		input[14] = 113;
		input[15] = 141;

		printk("pre output: "); hexdump(input, DATA_SIZE);

		ret = crypto_skcipher_decrypt(req);

		printk("pos output: "); hexdump(input, DATA_SIZE);
	}

	switch (ret) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		ret = wait_for_completion_interruptible(&result.completion);
		break;
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

static int __init cryptotest_init(void)
{
	printk("encrypt");
	do_test_cipher(0);
	printk("decrypt");
	do_test_cipher(1);

	return 0;
}

static void __exit cryptotest_exit(void)
{
}

module_init(cryptotest_init);
module_exit(cryptotest_exit);
