#include <linux/module.h>	// Mandatory for Modules
#include <linux/kernel.h>	// KERN_INFO
#include <linux/device.h>
#include <linux/init.h>		// MACROS
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL"); // removes the tainted module warning
MODULE_AUTHOR("Johann Meyer");
MODULE_DESCRIPTION("Exercise 4_4: PWM character driver");
MODULE_VERSION("0.1");

#define SUCCESS 0

#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

/***************************
 CHARACTER DEVICE PROPERTIES
****************************/

#define DEVICE_NAME "pwm"

static DEFINE_MUTEX(myMutex);
static int major;                 
static struct device *myDevice = NULL;
static struct class *myClass = NULL;

static int dev_open(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);
static int dev_release(struct inode *, struct file *);

static struct file_operations ops =
{
	.owner = THIS_MODULE, // for safe rmmodding
    .open = dev_open,
    .write = dev_write,
	.release = dev_release
};


/****************
 TIMER PROPERTIES
*****************/
#define PERIOD 10000000 // ns
//#define PERIOD_100 1E5L // ns
static struct hrtimer hr_timer;
static int toggle = 0;
/*
 use variables to avoid calculating these
 times each cycle.
*/
static long on_time = PERIOD; // ns
static long off_time = 0; // ns

enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer_for_restart)
{
	// get current time
	ktime_t curr_time, interval;
	curr_time = ktime_get();
	
	// toggle LED
	toggle = !toggle;
	if (toggle)
		interval = ktime_set(0, on_time);		
	else
		interval = ktime_set(0, off_time);		
	
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
	printk(KERN_INFO "4_4 INIT STARTING\n");

	/**************************
	 Set up GPIO pins
	**************************/

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

	/****************************
	 Set up character device
	****************************/
	
	// initialise mutex
	mutex_init(&myMutex);
	
    major = register_chrdev(0, DEVICE_NAME, &ops);
    if (major < 0)
    {
	    printk(KERN_ALERT "Failed to register a major number");
		return major; // driver failed to initialise
	}
	                                    
	// create device class
	myClass = class_create(THIS_MODULE, "4_4 Device Class");
	                                              
    // create device
    myDevice = device_create(myClass, NULL, MKDEV(major, 0),  
    						NULL, DEVICE_NAME);
	         
	/**************************
	 Set up timer
	***************************/
	// initialise hrtimer
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	
	// set callback function
	hr_timer.function = &my_hrtimer_callback;	
	
	// start hrtimer
	hrtimer_start(&hr_timer, ktime_set(1,0), HRTIMER_MODE_REL); 
	
	printk(KERN_INFO "4_4 INIT DONE\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "4_4 EXITING\n");
	
	/**********************
	 FREE TIMERS
	**********************/
		
	// cancel timer or ELSE
	hrtimer_cancel(&hr_timer);
	
	/*********************
	 FREE GPIO pins
	*********************/
	
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

	/***************************
	 FREE CHARACTER DEVICE
	***************************/

    // remove the device
    device_destroy(myClass, MKDEV(major, 0));
            
    // unregister device class
    class_unregister(myClass);
                       
    // remove device class
    class_destroy(myClass);
                                   
    // unregister major number
    unregister_chrdev(major, DEVICE_NAME);
                                            
    // destroy mutex
    mutex_destroy(&myMutex);
}

// MACROS to tell insmod and rmmod which functions to call
module_init(hello_init);
module_exit(hello_exit);

static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "opening device");
    
    /* 
     if file is already open return -EBUSY.
     Negative because EBUSY is positive and
     that is a valid file handle
    */
    if(!mutex_trylock(&myMutex))
    	return -EBUSY;
    	
    return SUCCESS;
}
                                           
static ssize_t dev_write(struct file *filep, const char __user *buffer,
						 size_t len, loff_t *offset) 
{
	/*
	 Safely get buffer from user space
	*/
	long ret;
	char kbuffer[len+1];
	ret = strncpy_from_user(kbuffer, buffer, len+1);
	
	if (ret > 0) // data was copied
	{
		/*
	 	 Convert to long
		*/
		long duty_cycle = 0;
    	ret = kstrtol(kbuffer, 10, &duty_cycle); // base of number system = 10
    	if (ret == 0) // succesfully converted to number
    	{
    		printk(KERN_INFO "%ld", duty_cycle);    
    		if (duty_cycle >= 0 && duty_cycle <= 100)
    		{
    			on_time = duty_cycle*PERIOD/100;
    			off_time = PERIOD-on_time;
				return len;
   			}
    	}
    }
    return -EINVAL; // input out of range or NaN
}
                                                   
static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "closing device");
    
    // release mutex
    mutex_unlock(&myMutex);

    return SUCCESS;
}
                
