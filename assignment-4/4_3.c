#include <linux/module.h>	// Mandatory for Modules
#include <linux/kernel.h>	// KERN_INFO
#include <linux/init.h>		// MACROS
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL"); // removes the tainted module warning
MODULE_AUTHOR("Johann Meyer");
MODULE_DESCRIPTION("Exercise 4_3: Hrtimer example");
MODULE_VERSION("0.1");

#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

static struct hrtimer hr_timer;
static int toggle = 0;

enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer_for_restart)
{
	// get current time
	ktime_t curr_time, interval;
	curr_time = ktime_get();
	
	// toggle LED
	toggle = !toggle;
	if (toggle)
		interval = ktime_set(1,0);		
	else
		interval = ktime_set(2,0);		
	
	// set LEDs
	gpio_set_value(LED1, toggle);
	gpio_set_value(LED2, toggle);
	gpio_set_value(LED3, toggle);
	gpio_set_value(LED4, toggle);

	// set new timer
	hrtimer_forward(timer_for_restart, curr_time, interval);
		
	return HRTIMER_RESTART;
}

static int __init hello_init(void)
{
	// request the gpio pins
	gpio_request(LED1, "led1");
	gpio_request(LED2, "led2");
	gpio_request(LED3, "led3");
	gpio_request(LED4, "led4");
	
	// set gpio direction
	gpio_direction_output(LED1,0);
	gpio_direction_output(LED2,0);
	gpio_direction_output(LED3,0);
	gpio_direction_output(LED4,0);

	// initialise hrtimer
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	
	// set callback function
	hr_timer.function = &my_hrtimer_callback;	
	
	// start hrtimer
	hrtimer_start(&hr_timer, ktime_set(1,0), HRTIMER_MODE_REL); 
	
	printk(KERN_INFO "Hello World\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye World\n");
	
	// cancel timer or ELSE
	hrtimer_cancel(&hr_timer);
	
	// switch LEDs off
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
