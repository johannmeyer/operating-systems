#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>

#include <pthread.h>

// Function prototype
void *ls_func(void *param);


int main()
{
	// create child thread to run commands
	pthread_t ls_thread;
	if (pthread_create(&ls_thread, NULL, &ls_func, NULL))
		perror("Thread Creation Failed\n");

	// rejoin child thread
	if(pthread_join(ls_thread, NULL))
		perror("Thread Joining Failed\n");
	printf("Parent thread done\n");

	return 0;
}

void *ls_func(void *param)
{
	printf("Child thread started\n");

	// C std library calls
	struct dirent **curr_dir;
	
	int n = scandir(".", &curr_dir, NULL, alphasort);
	if (n < 0)
	{		
		perror("scandir");
		return (void *)n;
	}
	else
	{
		int i = 2; // ignore . and ..
		for (; i < n; i++)
		{
			printf("%s  ", curr_dir[i]->d_name);
			free(curr_dir[i]);
		}
		free(curr_dir);
	}
	printf("\n");
	printf("Child thread done\n");
	return NULL;
}
