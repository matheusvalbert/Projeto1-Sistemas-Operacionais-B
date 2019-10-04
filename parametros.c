#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_AUTHOR("Crypto");
MODULE_DESCRIPTION("Simple CryptoAPI");
MODULE_LICENSE("GPL");

static char *key = "";
static char *iv = "";

module_param(key, charp, 0000);
MODULE_PARM_DESC(key, "A character string");
module_param(iv, charp, 0000);
MODULE_PARM_DESC(iv, "A character string");

static int __init cryptoapi_init(void) {

	printk("key: %s\n", key);
	printk("iv: %s\n", iv);

	return 0;
}

static void __exit cryptoapi_exit(void) {}

module_init(cryptoapi_init);
module_exit(cryptoapi_exit);
