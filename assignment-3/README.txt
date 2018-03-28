Assignment 3
------------
Thread Safety and Thread Synchronization.

3_1.c:	Example to illustrate the use of barriers using PThreads and Pulse
		Width Modulation (PWM).
3_2.c:	Thread-safe Queue Structure.
3_3.c:	Using the 3_2.c, four queues are generated (one for each LED). In
		total, there are 5 threads: 1x writer and 4x reader.
		USAGE: cat lights.txt | ./3_3
		Format of lights.txt: LED(0-3) Brightness(0-100) Duration(ms)
			e.g. 1 50 200
				 2 30 100
3_b:	This is composed of two .c files. One to illustrate starvation
		and the other to illustrate deadlock.
