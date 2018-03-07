#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
void *read_from_file(void *arg);

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
	
	// start input thread
	pthread_t pthread_input;
	pthread_create(&pthread_input, NULL, &input_func, data);

	// start file reading thread
	pthread_t pthread_io;
	pthread_create(&pthread_io, NULL, &read_from_file, &data->terminate);
	
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

	// get io thread to clean itself up first
	pthread_join(pthread_io, NULL);
	
	// free heap
	free(data);
}

void *input_func(void *v_data)
{
	struct data *data = (struct data *)v_data;
	int test_num;
	// note !terminate won't work here due to scanf
	// but this thread can just be killed w/o consequences
	while(1) // keep reading input
	{
		//read in int
		scanf("%d", &test_num);
		if (test_num == -1) // exit condition
		{
			// don't switch off lights here
			// it can lead to race condition!
			data->terminate = TRUE;
			return NULL; // exit would kill other thread
		}	
		else if (test_num < 0 || test_num > 15)
		{
			printf("Invalid Number\n");
		}
		else // set new target
		{
			data->target_num = test_num;
		}	
	}
}

void *read_from_file(void *arg)
{
	int *terminate = (int *)arg;
	
	double n;
	double cum_sum = 0;
	clock_t start, done;
	
	// open file
	FILE *stream = fopen("data.txt", "r");
	perror("fopen");
	if (stream == NULL)
	{
		// if file does not open let other
		// threads know and exit
		*terminate = TRUE;
		return NULL;
	}
	
	start = clock(); // start timer
	
	while(!*terminate && fscanf(stream, "%lf\n", &n) != EOF)
	{
		n = atan(tan(n));
		cum_sum += n;
	}
	
	if (!*terminate) // finished reading file
	{
		done = clock(); // computation done
		printf("Cumulative sum: %f\n", cum_sum);
		printf("Time taken: %f seconds\n", (done - start)/(double)CLOCKS_PER_SEC);
	}
	else
	{
		printf("Prematurely exited\n");
	}
	
	// close file
	fclose(stream);
	perror("fclose");
	return NULL;
}
