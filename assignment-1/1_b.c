#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#define MAX_LINE 80 // max length of command
#define HIST_SIZE 10 // history size

struct node
{
	int id;
	char raw_input[MAX_LINE+1];
	struct node *next;	
};

void tokenize(char *raw_input, char *(*args)[41], int *num_args);
void append(struct node **root, char *raw_input);
void print_list(struct node *root);
struct node *get_command(struct node *root, int id);
struct node *get_last_command(struct node *root);

int main(void)
{
	char *args[MAX_LINE/2+1]; // cl arguments
	int should_run = 1; // termination flag

	int status; // status of child process
	bool block;
	int num_args;
	char raw_input[MAX_LINE+1];
	char raw_input_destr[MAX_LINE+1]; // for tokenization
	struct node *root = NULL; // root of linkedlist
	
	while(should_run)
	{
		printf("osh>");
		fflush(stdout);

		/*
		 Get user input from keyboard
		*/
		// read line
		fgets(raw_input, MAX_LINE+1, stdin);
		
		strcpy(raw_input_destr, raw_input);	
		tokenize(raw_input_destr, &args, &num_args);
		
		/*
		 handle internal commands
		*/
		// handle empty line
		if(args[num_args-1] == NULL)
		{
			continue; // do nothing
		}
		
		// handle exit condition
		if (strcmp(args[0], "exit") == 0)
		{
			should_run = 0; // unnecessary because of break
			break; 
		}
		
		// handle history feature
		if (strcmp(args[0], "history") == 0)
		{
			print_list(root);
			continue;
		}		
		
		// !! command
		if (strcmp(args[0], "!!") == 0)
		{
			struct node *node = get_last_command(root);
			if (node == NULL)
			{
				printf("No commands in history\n");
				continue; // no history
			}
			printf("%s", node->raw_input); // echo command
			strcpy(raw_input, node->raw_input); // for history
			strcpy(raw_input_destr, raw_input); // for tokenization
			tokenize(raw_input_destr, &args, &num_args);
		}
		else if (args[0][0] == '!') // !N command
		{
			int cmd_num;			
			int ret = sscanf(args[0], "%*[!]%d", &cmd_num);
			
			if (ret <= 0) // no match
			{
				printf("Invalid command\n");
				continue;
			}

			struct node *node = get_command(root, cmd_num);
			if (node == NULL)
			{
				printf("No such command in history\n");
				continue;
			}
			printf("%s", node->raw_input); // echo command
			strcpy(raw_input, node->raw_input); // for history
			strcpy(raw_input_destr, raw_input); // for tokenization
			tokenize(raw_input_destr, &args, &num_args);
		}
		
		// add to history
		append(&root, raw_input);			
					
		// handle background condition
		if (strcmp(args[num_args-1],"&") == 0)
		{
			block = false;
			args[--num_args] = NULL; // delete &
		}
		else
		{
			block = true;
		}
	
		// Non-blocking wait to collect zombies
		while(waitpid(-1, &status, WNOHANG) > 0);
		
		/*
		 fork child process
		*/
		pid_t pid = fork();
		if (pid == 0) // child
		{
			// get child to run command
			execvp(args[0], args);
			perror(args[0]);
			return 0;
		}
		else // parent
		{
			// handle case of &
			if (block)
			{
				pid_t ret = waitpid(pid, &status, 0);
				if (ret == -1)
					perror("waitpid failed");
			}
		}
	}
	
	// free linked list
	struct node *node = root;
	struct node *prev;
	while (node != NULL)
	{
		prev = node;
		node = node->next;
		free(prev);
	}
	
	return 0;
}

void tokenize(char *raw_input, char *(*args)[41], int *num_args)
{
	/*
	 Note tokenization using strtok is a destructive operation
	*/
	// tokenize string
	*num_args = 0;
	(*args)[*num_args] = strtok(raw_input, " \n");
	while(((*args)[++(*num_args)] = strtok(NULL, " \n")) != NULL);
}

void print_list(struct node *root)
{
	/*
	 prints the linked list
	*/
	struct node *node = root;
	
	if (root == NULL)
	{
		printf("No commands in history\n");
		return;
	}
	
	while(node!=NULL)
	{
		printf("%d %s", node->id, node->raw_input); 
		node = node->next;
	}
}

void append(struct node **root, char *raw_input)
{
	/*
	 appends the raw_input to the history of the commands
	*/

	// start at one since while loop stops one iteration early
	int num_items = 1;
	
	// initialize list
	if (*root == NULL)
	{
		*root = malloc(sizeof(struct node));
		(*root)->id = 1;
		strcpy((*root)->raw_input, raw_input);
		(*root)->next = NULL;
		return;
	}
	
	struct node *node = *root;

	// find tail
	while(node->next != NULL)
	{
		num_items ++;
		node = node->next;
	}
		
	// append in cyclic fashion
	if (num_items == HIST_SIZE) // list is full (reuse root)
	{
		node->next = *root;
		*root = (*root)->next;
	}
	else // fill up list
	{
		node->next = malloc(sizeof(struct node));
	}
	
	// set the values for the new entry
	int id = node->id +1;
	node = node->next;
	node->id = id;
	strcpy(node->raw_input, raw_input);
	node->next = NULL;
}

struct node *get_command(struct node *root, int id)
{
	struct node *node = root;
	while (node != NULL && node->id <= id)
	{
		if (node->id == id)
			return node; // command found
		node = node->next;
	}
	return NULL; // couldn't find command
}

struct node *get_last_command(struct node *root)
{
	struct node *node = root;
	if (root == NULL)
	{
		return NULL; // no commands in history
	}
	
	// find tail
	while (node->next != NULL)
		node = node->next;
	return node; // return tail
}
