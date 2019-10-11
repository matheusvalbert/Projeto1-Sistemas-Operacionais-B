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

#define MODNAME "ciphertest"

#define DEBUG
#define TEST_HASH

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
	pr_debug("%s Encryption finished successfully\n", MODNAME);
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
	int ret;
	char key[16] = {"0123456789abcdef"};
	char iv[16] = {"0123456789abcdef"};
	int key_size = 16;
	char *input = NULL;
	struct tcrypt_result result;

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


	ret = crypto_skcipher_setkey(tfm, key, key_size);
	if (ret != 0) {
		pr_err("ERROR: crypto_skcipher_setkey\n");
		goto error_req;
	}

	input = kmalloc(16, GFP_KERNEL);
        if (!input) {
                printk("kmalloc(input) failed\n");
                goto out;
        }

	pr_debug("%s: IV %d %zd\n", MODNAME, crypto_skcipher_ivsize(tfm),
		 strlen(iv));

	sg_init_one(&sg, input, 16);

	skcipher_request_set_crypt(req, &sg, &sg, 16, iv);
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

		printk("pre input: "); hexdump(input, 16);

		ret = crypto_skcipher_encrypt(req);

		printk("pos input: "); hexdump(input, 16);
	}
	else {

		input[0] = 89;
		input[1] = 178;
		input[2] = 79;
		input[3] = 207;
		input[4] = 1;
		input[5] = 151;
		input[6] = 106;
		input[7] = 202;
		input[8] = 233;
		input[9] = 99;
		input[10] = 152;
		input[11] = 27;
		input[12] = 166;
		input[13] = 87;
		input[14] = 61;
		input[15] = 51;

		printk("pre output: "); hexdump(input, 16);

		ret = crypto_skcipher_decrypt(req);

		printk("pos output: "); hexdump(input, 16);
	}

	/*ret = crypto_skcipher_decrypt(req);*/
	switch (ret) {
	case 0:
		pr_debug("%s: OK\n", MODNAME);
		break;
	case -EINPROGRESS:
	case -EBUSY:
		pr_debug("%s: On wait\n", MODNAME);
		ret = wait_for_completion_interruptible(&result.completion);
		break;
	default:
		pr_info("%s: DEFAULT\n", MODNAME);
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

MODULE_AUTHOR("Corentin LABBE <clabbe.montjoie@gmail.com>");
MODULE_DESCRIPTION("test and bench encryption / decryption");
MODULE_LICENSE("GPL");

