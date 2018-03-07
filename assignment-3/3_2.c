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


void initQueue(struct Queue *queue);
void addToQueue(struct Queue *queue, int brightness, int duration);
void removeFromQueue(struct Queue *queue, int *pBrightness, int *pDuration);
int queueSize(struct Queue *queue);
void *add100000(void *v_queue);
void *remove100000(void *v_queue);

int main ()
{

	// allocate queue
	struct Queue *queue = malloc(sizeof(struct Queue));
	initQueue(queue);
		
	// create threads to add to queue
	pthread_t th_1, th_2;
	pthread_create(&th_1, NULL, &add100000, queue);
	pthread_create(&th_2, NULL, &add100000, queue);
	
	// collect threads to avoid premature count determination
	pthread_join(th_1, NULL);
	pthread_join(th_2, NULL);
	
	// determine size of queue
	int count = queueSize(queue);
	printf("Queue size: %d\n", count);
	
	// create threads to remove from queue
	pthread_create(&th_1, NULL, &remove100000, queue);
	pthread_create(&th_2, NULL, &remove100000, queue);
	
	// collect threads to avoid premature termination
	// due to main thread exiting
	pthread_join(th_1, NULL);
	pthread_join(th_2, NULL);
	
	// determine size of queue
	count = queueSize(queue);
	printf("Queue size: %d\n", count);
	
	// release the memory held by the mutex
	pthread_mutex_destroy(&queue->mutex);
	
	// ideally loop through queue and free if !empty
	
	// free memory
	free(queue);
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


void *add100000(void *v_queue)
{
	struct Queue *queue = v_queue;
		
	int count = 0;
	while (count < 100000)
	{
		addToQueue(queue, 0, 0);
		count++;
	}
	
	return NULL;
}

void *remove100000(void *v_queue)
{
	struct Queue *queue = v_queue;
		
	int count = 0;
	int brightness, duration;
	while (count < 100000)
	{
		removeFromQueue(queue, &brightness, &duration);
		if (brightness == -1 || duration == -1)
		{
			printf("-1 occurred\n");
		}
		count++;
	}
	
	return NULL;
}
