#include <unistd.h>
#include <stdio.h>

int main()
{
	char name[] = "Johann Meyer";
	int student_number = 4303113; 
	printf("My name: %s\n", name);
	printf("My student number: %d\n", student_number);
	printf("My process ID: %d\n",getpid());
	return 0;
}
