#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

void *brighten(void *v_barrier);
void *dim(void *v_barrier);

int main ()
{
	wiringPiSetup();

	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);

	// setup lighting system
	softPwmCreate(LED1, 0, 100);
	softPwmCreate(LED2, 0, 100);
	softPwmCreate(LED3, 0, 100);
	softPwmCreate(LED4, 0, 100);

	
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, 2);
	// create threads to brighten and dim LEDs
	pthread_t th_brighten, th_dim;
	pthread_create(&th_brighten, NULL, &brighten, &barrier);
	pthread_create(&th_dim, NULL, &dim, &barrier);
	
	// collect threads to avoid premature termination
	// due to main thread exiting
	pthread_join(th_brighten, NULL);
	pthread_join(th_dim, NULL);
	
	// release the memory held by the barrier
	pthread_barrier_destroy(&barrier);
}

void *brighten(void *v_barrier)
{
	pthread_barrier_t *barrier = v_barrier;
	
	printf("brighten\n");
	
	int brightness = 0;
	while (brightness < 100)
	{
		softPwmWrite(LED1,brightness);
		softPwmWrite(LED2,brightness);
		softPwmWrite(LED3,brightness);
		softPwmWrite(LED4,brightness);
		brightness += 10;
		usleep(200000);
	}
	
	pthread_barrier_wait(barrier);
	return NULL;
}

void *dim(void *v_barrier)
{
	pthread_barrier_t *barrier = v_barrier;
	pthread_barrier_wait(barrier);
	
	printf("dim\n");

	int brightness = 100;
	while (brightness > 0)
	{
		softPwmWrite(LED1,brightness);
		softPwmWrite(LED2,brightness);
		softPwmWrite(LED3,brightness);
		softPwmWrite(LED4,brightness);
		brightness -= 10;
		usleep(200000);
	}
	return NULL;
}
