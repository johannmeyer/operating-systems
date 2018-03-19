#include <linux/module.h>	// Mandatory for Modules
#include <linux/kernel.h>	// KERN_INFO
#include <linux/init.h>		// MACROS
#include <linux/gpio.h>

MODULE_LICENSE("GPL"); // removes the tainted module warning
MODULE_AUTHOR("Johann Meyer");
MODULE_DESCRIPTION("Exercise 4_1: manipulates LEDS using GPIO pins.");
MODULE_VERSION("0.1");

#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

static int __init hello_init(void)
{
	// request the gpio pins
	gpio_request(LED1, "led1");
	gpio_request(LED2, "led2");
	gpio_request(LED3, "led3");
	gpio_request(LED4, "led4");
	
	// set gpio direction
	gpio_direction_output(LED1,1);
	gpio_direction_output(LED2,1);
	gpio_direction_output(LED3,1);
	gpio_direction_output(LED4,1);
	
	printk(KERN_INFO "Hello World\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye World\n");
	
	// switch leds off
	gpio_set_value(LED1,0);
	gpio_set_value(LED2,0);
	gpio_set_value(LED3,0);
	gpio_set_value(LED4,0);

	// release the gpio pins
	gpio_free(LED1);
	gpio_free(LED2);
	gpio_free(LED3);
	gpio_free(LED4);
}

// MACROS to tell insmod and rmmod which functions to call
module_init(hello_init);
module_exit(hello_exit);
