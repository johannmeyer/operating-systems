#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

struct data
{
	int target_num;
	int terminate;
};

void *input_func(void *v_data);

int main(int argc, char**args)
{

	if (argc != 2)
	{
		printf("Please add the number as cmd argument.\n");
		exit(1);
	}

	wiringPiSetup();
	
	// set GPIO pin modes
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	
	// switch off LEDS
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	digitalWrite(LED3, LOW);
	digitalWrite(LED4, LOW);
	
	sleep(1);
	
	// create data to be shared between threads.
	struct data *data= malloc(sizeof(struct data));
	
	//convert string arg to int
	data->target_num = atoi(args[1]);
	data->terminate = FALSE;
	
	if (data->target_num < 0 || data->target_num > 15)
	{
		printf("Invalid Number\n");
		exit(1);
	}
	
	pthread_t pthread_input;
	pthread_create(&pthread_input, NULL, &input_func, data);
	
	int curr_num = 0;
	while(!data->terminate)	
	{
		if (data->target_num != curr_num)
		{
			if (data->target_num > curr_num)
				curr_num++;
			else
				curr_num--;
				
			if(curr_num & (1<<3))
				digitalWrite(LED4, HIGH);
			else
				digitalWrite(LED4, LOW);
			if(curr_num & (1<<2))	
				digitalWrite(LED3, HIGH);
			else
				digitalWrite(LED3, LOW);
			if(curr_num & (1<<1))
				digitalWrite(LED2, HIGH);
			else
				digitalWrite(LED2, LOW);
			if(curr_num & (1<<0))
				digitalWrite(LED1, HIGH);
			else
				digitalWrite(LED1, LOW);
			usleep(200000); // 200ms delay
		}	
	}
	
	// switch off LEDS
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	digitalWrite(LED3, LOW);
	digitalWrite(LED4, LOW);
	
	free(data);
}

void *input_func(void *v_data)
{
	struct data *data = (struct data *)v_data;
	
	while(1) // keep reading input
	{
		//read in int
		scanf("%d", &data->target_num);

		if (data->target_num == -1) // exit condition
		{
			// don't switch off lights here
			// it can lead to race condition!
			data->terminate = TRUE;
			return NULL; // exit would kill other thread
		}	
		else if (data->target_num < 0 || data->target_num > 15)
		{
			printf("Invalid Number\n");
		}	
	}
}
