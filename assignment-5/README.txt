Assignment 5
------------
This assignment deals with security aspects related to Operating Systems.

5_1.c: 	Source code of the exploitable program. 
	   	Contains a hidden unused function.
5_1.sh: This script exploits strcpy by overflowing the buffer and 
		executing the hidden function.
5_b.sh: This script exploits strcpy to execute a shell using Return-oriented
		Programming (ROP). This exploit requires Address Space Layout
		Randomization to be disabled. Part of the script but not the exploit.
5_2.c: 	Source code for a secure program loader. It uses openssl and objcopy
	   	to extract the embedded sha256 code and verify the executable before 
	   	executing it.
