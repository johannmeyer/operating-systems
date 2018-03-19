#include <linux/module.h>	// Mandatory for Modules
#include <linux/kernel.h>	// KERN_INFO
#include <linux/device.h>
#include <linux/init.h>		// MACROS
#include <linux/mutex.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL"); // removes the tainted module warning
MODULE_AUTHOR("Johann Meyer");
MODULE_DESCRIPTION("A character device that generates prime numbers.");
MODULE_VERSION("0.1");

/*************************
 Device file properties
**************************/

#define DEVICE_NAME "prime"
#define SUCCESS 0
#define MAX_DIGITS 10

static DEFINE_MUTEX(myMutex);
static int major;
static struct device *myDevice = NULL;
static struct class *myClass = NULL;

static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);
static int dev_release(struct inode *, struct file *);

static struct file_operations ops =
{
	.owner = THIS_MODULE, // handles rmmodding while file open
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release
};

static long curr_num = 0;

static int __init hello_init(void)
{
	printk(KERN_INFO "Prime char device driver init\n");
	
	// initialise mutex
	mutex_init(&myMutex);

	// register character device
	major = register_chrdev(0, DEVICE_NAME, &ops);
	if (major < 0)
	{
		printk(KERN_ALERT "Failed to register a major number");
		return major; // driver failed to initialise
	}
	
	// create device class
	myClass = class_create(THIS_MODULE, "4_b Device Class");
	
	// create device
	myDevice = device_create(myClass, NULL, MKDEV(major, 0),
							 NULL, DEVICE_NAME);
	
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Prime char device driver exit\n");

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

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
	int ret; 
	char kbuffer[MAX_DIGITS+2]; // +2 '\n\0'

	if (*offset == 0)
	{
		if (curr_num < 2)
			curr_num = 2; // 0 and 1 != prime
		
		curr_num--; // include current number in check
	
		int prime = 0;
		while (!prime)
		{
			int i;
			curr_num++;
			prime = 1;
			for(i = 2; i < curr_num/2+1; i++)
				if(curr_num % i == 0)
					prime = 0;
		}		
	}
	snprintf(kbuffer, MAX_DIGITS, "%ld\n", curr_num);
			
	ret = put_user(kbuffer[*offset], buffer);
	if (ret != SUCCESS)
		return -EINVAL;
	if (kbuffer[*offset] != '\n')
	{
		(*offset)++;
	}
	else
	{
		curr_num++; // start from next number
		*offset = 0;
	}
	return 1;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
	long ret;
	char kbuffer[len+1]; // +1 for null byte
	/*
	 check validity of user space buffer (security) and copy into kbuffer
	 to avoid the problem if the user space buffer is paged out
	 of physical memory (memcpy).
	*/
	ret = strncpy_from_user(kbuffer, buffer, len+1);
	if (ret > 0)
	{
		long input;
		ret = kstrtol(kbuffer, 10, &input);
		if (ret == SUCCESS && input >= 0)
		{
			printk(KERN_INFO "Wrote %ld to %s\n", input, DEVICE_NAME);
			curr_num = input;
			return len;
		}
	}
	
	printk(KERN_ALERT "Invalid Address/Value\n");
	return -EINVAL;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "closing device");

	// release mutex
	mutex_unlock(&myMutex);
	
	return SUCCESS;
}
