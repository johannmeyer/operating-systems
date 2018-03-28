Assignment 2
------------
Multithreading and LEDs.

2_1.c:	Display the input number in binary using the LEDs.
		USAGE: ./2_1 11
2_2.c:	Counts to the input number and has a listener thread that reads
		input from the keyboard and counts to the new number. Exit
		by entering -1.
		USAGE: ./2_2 11
2_3.c:	Same as 2_2.c but an addtional thread will perform IO and perform
		operations on the values read from the file.
2_4.c:	Example using busy_wait (bad) instead of usleep. This program
		must be run as sudo and will change the scheduler of the Kernel to
		use round-robin. Higher-priority for the IO thread will lead to
		starvation of the other threads.
2_b.c:	A multi-threaded sudoku solution validator. It has one thread per
		column, row, and block. In total, 27 threads.
