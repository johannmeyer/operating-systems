#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *function(void *arg);

int main()
{
	// create mutex
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

	// create starved thread
	pthread_t starved_thread;
	pthread_create(&starved_thread, NULL, &function, &mutex); 

	//greedy loop
	pthread_mutex_lock(&mutex);
	while(1)
	{
		printf("Main thread busy\n");
		usleep(1000000);
	}
	pthread_mutex_unlock(&mutex);

	// destroy mutex
	pthread_mutex_destroy(&mutex);
}

void *function(void *arg)
{
	pthread_mutex_t *mutex = arg;
	
	while(1)
	{
		pthread_mutex_lock(mutex);
		printf("Starving thread\n");
		pthread_mutex_unlock(mutex);
	}
}
