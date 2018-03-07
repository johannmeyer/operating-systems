#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

struct Node
{
	int brightness, duration;
	struct Node *next;
};

struct Queue
{
	struct Node *front;
	struct Node *back;
	pthread_mutex_t mutex;
};

struct ConsumerArgs
{
	struct Queue *queue;
	int led;
	int *terminate;
};

struct ProducerArgs
{
	struct Queue *queues;
	int terminate;
};

void initQueue(struct Queue *queue);
void addToQueue(struct Queue *queue, int brightness, int duration);
void removeFromQueue(struct Queue *queue, int *pBrightness, int *pDuration);
int queueSize(struct Queue *queue);
void *consumer(void *v_consumer_args);
void *producer(void *v_producer_args);

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

	// allocate queues
	struct Queue *queues = malloc(4*sizeof(struct Queue));
	initQueue(queues);
	initQueue(queues+1);
	initQueue(queues+2);
	initQueue(queues+3);
			
	// create producer thread
	pthread_t t_producer;
	struct ProducerArgs producer_args;
	producer_args.terminate = 0;
	producer_args.queues = queues;
	pthread_create(&t_producer, NULL, &producer, &producer_args);
	
	// create consumer threads
	pthread_t t_consumers[4];
	struct ConsumerArgs consumer_args[4];
	consumer_args[0].led = LED1;
	consumer_args[0].queue = queues;
	consumer_args[0].terminate = &producer_args.terminate;
	pthread_create(t_consumers, NULL, &consumer, consumer_args);
	consumer_args[1].led = LED2;
	consumer_args[1].queue = queues+1;
	consumer_args[1].terminate = &producer_args.terminate;
	pthread_create(t_consumers+1, NULL, &consumer, consumer_args+1);
	consumer_args[2].led = LED3;
	consumer_args[2].queue = queues+2;
	consumer_args[2].terminate = &producer_args.terminate;
	pthread_create(t_consumers+2, NULL, &consumer, consumer_args+2);
	consumer_args[3].led = LED4;
	consumer_args[3].queue = queues+3;
	consumer_args[3].terminate = &producer_args.terminate;
	pthread_create(t_consumers+3, NULL, &consumer, consumer_args+3);
	
	// collect all threads
	pthread_join(t_producer, NULL);
	pthread_join(t_consumers[0], NULL);
	pthread_join(t_consumers[1], NULL);
	pthread_join(t_consumers[2], NULL);
	pthread_join(t_consumers[3], NULL);
		
	// release the memory held by the mutex
	int i;
	for(i = 0; i < 4; i++)
		pthread_mutex_destroy(&queues[i].mutex);
		
	// free memory
	free(queues);
}

void initQueue(struct Queue *queue)
{
	queue->front = NULL;
	queue->back = NULL;
	pthread_mutex_init(&queue->mutex, NULL);
}

void addToQueue(struct Queue *queue, int brightness, int duration)
{
	// allocate mem for new node
	struct Node *node = malloc(sizeof(struct Node));
	
	// set properties of new node
	node->brightness = brightness;
	node->duration = duration;
	node->next= NULL;
	
	// obtain lock on queue
	pthread_mutex_lock(&queue->mutex);
		
	if (queue->back != NULL)
	{
		// add to the back of the queue
		struct Node *back = queue->back;
		// update last element
		back->next = node;
		// update the queue
		queue->back = node;
	}
	else // add first element to list
	{
		queue->front = node;
		queue->back = node;
	}
	
	// release lock on queue
	pthread_mutex_unlock(&queue->mutex);
}

void removeFromQueue(struct Queue *queue, int *pBrightness, int *pDuration)
{
	// obtain lock on queue
	pthread_mutex_lock(&queue->mutex);

	if (queue->front != NULL)
	{
		// get element at front of queue
		struct Node *node = queue->front;
	
		// get values
		*pBrightness = node->brightness;
		*pDuration = node->duration;
	
		// remove from queue
		queue->front = node->next;
		
		// free memory
		free(node);
	}
	else // queue is empty
	{
		*pBrightness = -1;
		*pDuration = -1;
	}
	
	// release lock on queue
	pthread_mutex_unlock(&queue->mutex);
}

// return length of queue
int queueSize(struct Queue *queue)
{
	int count = 0;
	
	// traverse the linked list
	struct Node *curr_node = queue->front;
	while (curr_node != NULL)
	{
		count ++;
		curr_node = curr_node->next;
	}
	return count;
}


void *consumer(void *v_consumer_args)
{
	printf("created consumer\n");
	struct ConsumerArgs *consumer_args = v_consumer_args;

	struct Queue *queue = consumer_args->queue;
	int led = consumer_args->led;
	int *terminate = consumer_args->terminate;
	int brightness, duration;
	while (1)
	{
		removeFromQueue(queue, &brightness, &duration);
		if (brightness != -1 && duration != -1)
		{
			softPwmWrite(led,brightness);
			usleep(duration*1000);
			softPwmWrite(led,0);
		}
		else if (*terminate && queueSize(queue)==0)
		{
			/*
			 Queue is empty and producer no longer writing to the queues.
			 
			 Note order of conditions in if statement is critical to avoid
			 race condition if a context switch etc happens between the &&
			*/
			break;
		}	
	}
	
	return NULL;
}

void *producer(void *v_producer_args)
{
	printf("created producer\n");
	struct ProducerArgs *producer_args = v_producer_args;
	struct Queue *queues = producer_args->queues;
		
	int queue_offset, brightness, duration;
	while (scanf("%d %d %d", &queue_offset, &brightness, &duration) != EOF)
	{
		if (queue_offset < 4 && queue_offset >= 0)
			addToQueue(queues+queue_offset, brightness, duration);
		else
			printf("invalid input\n");
	}
	
	// communicate to consumers that it is done
	producer_args->terminate = 1;
	printf("producer done\n");
	return NULL;
}
