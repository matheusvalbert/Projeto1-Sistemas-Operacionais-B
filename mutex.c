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
#include <linux/mutex.h>	         /// Required for the mutex functionalit

static DEFINE_MUTEX(ebbchar_mutex);  /// A macro that is used to declare a new mutex that is visible in this file
                                     /// results in a semaphore variable ebbchar_mutex with value 1 (unlocked)
                                     /// DEFINE_MUTEX_LOCKED() results in a variable with value 0 (locked)

static int __init
init_cryptoapi_demo(void)
{

	mutex_init(&ebbchar_mutex);
	
	if(!mutex_trylock(&ebbchar_mutex)){    /// Try to acquire the mutex (i.e., put the lock on/down)
                                          /// returns 1 if successful and 0 if there is contention
      		printk(KERN_ALERT "EBBChar: Device in use by another process");
      		return -EBUSY;
   	}
	else {
		printk("Mutex locked");
	}

        return 0;
}


static void __exit
exit_cryptoapi_demo(void)
{
	mutex_unlock(&ebbchar_mutex);

	mutex_destroy(&ebbchar_mutex);
}

module_init(init_cryptoapi_demo);
module_exit(exit_cryptoapi_demo);
