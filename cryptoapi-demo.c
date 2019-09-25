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
#include <asm/scatterlist.h>

#define PFX "cryptoapi-demo: "

MODULE_AUTHOR("Michal Ludvig <michal@logix.cz>");
MODULE_DESCRIPTION("Simple CryptoAPI demo");
MODULE_LICENSE("GPL");

/* ====== CryptoAPI ====== */

#define DATA_SIZE       16

#define FILL_SG(sg,ptr,len)     do { (sg)->page = virt_to_page(ptr); (sg)->offset = offset_in_page(ptr); (sg)->length = len; } while (0)

static void
hexdump(unsigned char *buf, unsigned int len)
{
        while (len--)
                printk("%02x", *buf++);

        printk("\n");
}

static void
cryptoapi_demo(void)
{
        /* config options */
        char *algo = "aes";
        int mode = CRYPTO_TFM_MODE_CBC;
        char key[16], iv[16];

        /* local variables */
        struct crypto_tfm *tfm;
        struct scatterlist sg[8];
        int ret;
        char *input, *encrypted, *decrypted;

        memset(key, 0, sizeof(key));
        memset(iv, 0, sizeof(iv));

        tfm = crypto_alloc_tfm (algo, mode);

        if (tfm == NULL) {
                printk("failed to load transform for %s %s\n", algo, mode == CRYPTO_TFM_MODE_CBC ? "CBC" : "");
                return;
        }

        ret = crypto_cipher_setkey(tfm, key, sizeof(key));

        if (ret) {
                printk(KERN_ERR PFX "setkey() failed flags=%x\n", tfm->crt_flags);
                goto out;
        }

        input = kmalloc(GFP_KERNEL, DATA_SIZE);
        if (!input) {
                printk(KERN_ERR PFX "kmalloc(input) failed\n");
                goto out;
        }

        encrypted = kmalloc(GFP_KERNEL, DATA_SIZE);
        if (!encrypted) {
                printk(KERN_ERR PFX "kmalloc(encrypted) failed\n");
                kfree(input);
                goto out;
        }

        decrypted = kmalloc(GFP_KERNEL, DATA_SIZE);
        if (!decrypted) {
                printk(KERN_ERR PFX "kmalloc(decrypted) failed\n");
                kfree(encrypted);
                kfree(input);
                goto out;
        }

        memset(input, 0, DATA_SIZE);

        FILL_SG(&sg[0], input, DATA_SIZE);
        FILL_SG(&sg[1], encrypted, DATA_SIZE);
        FILL_SG(&sg[2], decrypted, DATA_SIZE);

        crypto_cipher_set_iv(tfm, iv, crypto_tfm_alg_ivsize (tfm));
        ret = crypto_cipher_encrypt(tfm, &sg[1], &sg[0], DATA_SIZE);
        if (ret) {
                printk(KERN_ERR PFX "encryption failed, flags=0x%x\n", tfm->crt_flags);
                goto out_kfree;
        }

        crypto_cipher_set_iv(tfm, iv, crypto_tfm_alg_ivsize (tfm));
        ret = crypto_cipher_decrypt(tfm, &sg[2], &sg[1], DATA_SIZE);
        if (ret) {
                printk(KERN_ERR PFX "decryption failed, flags=0x%x\n", tfm->crt_flags);
                goto out_kfree;
        }

        printk(KERN_ERR PFX "IN: "); hexdump(input, DATA_SIZE);
        printk(KERN_ERR PFX "EN: "); hexdump(encrypted, DATA_SIZE);
        printk(KERN_ERR PFX "DE: "); hexdump(decrypted, DATA_SIZE);

        if (memcmp(input, decrypted, DATA_SIZE) != 0)
                printk(KERN_ERR PFX "FAIL: input buffer != decrypted buffer\n");
        else
                printk(KERN_ERR PFX "PASS: encryption/decryption verified\n");

out_kfree:
        kfree(decrypted);
        kfree(encrypted);
        kfree(input);

out:
        crypto_free_tfm(tfm);
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
