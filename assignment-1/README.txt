Assignment 1
------------
Processes

1_1.c:	Prints some details and the process ID.
1_2.c:	Creates a directory and lists contents of a directory using two
		different methods. (Directly executing shell functions and libc
		calls)
1_3.c: 	Create a child process and make parent wait for it two finish
		to reap it (no zombie process). The child will show all files
		and folders in the directory.
1_4.c:	Same as 1_3.c but with PThreads instead.
1_b.c:	Basic Unix shell with a history feature and background processes.
		USAGE: !! is most recent command. !N where N is a number executes
		the Nth command. 'history' will print the command history (by
		default last 10 commands).
