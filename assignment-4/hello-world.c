#include <linux/module.h>	// Mandatory for Modules
#include <linux/kernel.h>	// KERN_INFO
#include <linux/init.h>		// MACROS

MODULE_LICENSE("GPL"); // removes the tainted module warning
MODULE_AUTHOR("Johann Meyer");
MODULE_DESCRIPTION("Hello World Example");
MODULE_VERSION("0.1");

/*
 __initdata means kernel will free the variable 
 after hello_init completes. This ensures the memory
 footprint of the module remains as small as possible.
 static declaration also ensures that only this file
 can see it and not all modules.
*/
static int lucky_number __initdata = 7; 

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello World %d\n", lucky_number);
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye World\n");
}

// MACROS to tell insmod and rmmod which functions to call
module_init(hello_init);
module_exit(hello_exit);
