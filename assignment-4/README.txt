Assignment 4
------------
Linux Kernel.
Note: all modules are thread-safe.

4_1.c:	Basic Linux Kernel Module to switch on LEDs using Linux's GPIO.h.
4_2.c:	Simple Linux Character Device Driver at /dev/klog. It accepts string input
		and prints this string to the kernel log.
		USAGE: sudo sh -c 'echo "MESSAGE" > /dev/klog'
4_3.c:	Linux Kernel Module utilising Linux's hrtimer (High-resolution timer)
		It toggles the LEDs on for 1 sec and off for 2 secs, while the module
		is loaded.
4_4.c:	This character device driver is registered at /dev/pwm. Writing a
		number between 0-100 changes the duty cycle of the Pulse Width
		Modulation (PWM). It effectively dims or brightens the LED.
4_b.c:	This character device driver is registered at /dev/prime. Reading
		from it will generate the next prime number followed by a '\n'.
		Writing to the driver will make the driver find the next prime
		number from that number onwards.
