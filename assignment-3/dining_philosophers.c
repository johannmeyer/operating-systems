/*
 This program is an example of how the dining philosophers
 problem by Dijkstra can cause a deadlock. Can be done with
 just two threads but assignment asked for 3 threads.
*/

#include <stdio.h>
#include <pthread.h>

struct PhilosopherArgs
{
	pthread_mutex_t *fork1, *fork2;
	pthread_barrier_t *barrier; // this will be used to guarantee deadlock
};

void *philosopher(void *args);

int main()
{
	struct PhilosopherArgs philosopher_args1, philosopher_args2,
						   philosopher_args3;
		
	// initialise barrier
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, 3);
	
	// initialise mutexes
	pthread_mutex_t fork1, fork2, fork3;
	pthread_mutex_init(&fork1, NULL);
	pthread_mutex_init(&fork2, NULL);
	
	// copy the barrier
 	philosopher_args1.barrier = &barrier;
	philosopher_args2.barrier = &barrier;
	philosopher_args3.barrier = &barrier;

	// swap forks for 2nd philosopher since he faces opposite direction
	philosopher_args1.fork1 = &fork1;
	philosopher_args1.fork2 = &fork2;
	philosopher_args2.fork1 = &fork2;
	philosopher_args2.fork2 = &fork3;
	philosopher_args3.fork1 = &fork3;
	philosopher_args3.fork2 = &fork1;
	
	// create threads
	pthread_t philosopher1, philosopher2, philosopher3;
	pthread_create(&philosopher1, NULL, &philosopher, &philosopher_args1);
	pthread_create(&philosopher2, NULL, &philosopher, &philosopher_args2);
	pthread_create(&philosopher3, NULL, &philosopher, &philosopher_args3);
	
	//This will never return because of deadlock
	pthread_join(philosopher1, NULL);
	pthread_join(philosopher2, NULL);
	pthread_join(philosopher3, NULL);
	printf("No deadlock occurred!\n");

	// destroy mutexes
	pthread_mutex_destroy(&fork1);
	pthread_mutex_destroy(&fork2);
	
	// destroy barrier
	pthread_barrier_destroy(&barrier);
}

void *philosopher(void *args)
{
	struct PhilosopherArgs *philosopher_args = args;
	
	// pick up fork1
	printf("picking up left fork\n");
	pthread_mutex_lock(philosopher_args->fork1);
	printf("got left fork\n");
	
	// wait for everyone to have left fork in hand now 
	pthread_barrier_wait(philosopher_args->barrier);

	// pick up fork2
	printf("picking up right fork\n");
	pthread_mutex_lock(philosopher_args->fork2);
	printf("got right fork\n");
	
	printf("I have two forks\n");
	
	// now release the forks
	pthread_mutex_unlock(philosopher_args->fork1);
	pthread_mutex_unlock(philosopher_args->fork2);

	return NULL;
}
