#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct params
{
    int *data; // pointer to sudoku values
    int  loc;  // 0-8 can be row, col, or block
};

void *row_checker(void *arg);
void *col_checker(void *arg);
void *block_checker(void *arg);

int main(void)
{
    int i; // general counter
    // store values of sudoku
    int *data = malloc(81 * sizeof(int));
    if (data == NULL)
    {
        printf("malloc failed: data\n");
        exit(1);
    }

    // open file
    FILE *file = fopen("sudoku.txt", "r");
    if (file == NULL)
    {
        printf("File sudoku.txt could not be opened.\n");
        exit(1);
    }

    // read file
    i = 0;
    while (fscanf(file, "%d", data + i) != EOF)
        i++;

    // close file
    fclose(file);

    /*
    Create Threads
     */
    // declare thread handles for each thread
    pthread_t rows[9];
    pthread_t cols[9];
    pthread_t blocks[9];

    struct params *param = malloc(9 * sizeof(struct params));
    if (param == NULL)
    {
        printf("malloc failed: param\n");
        exit(1);
    }
    int ret;
    for (i = 0; i < 9; i++)
    {
        param[i].data = data;
        param[i].loc = i;

        ret = pthread_create(rows + i, NULL, &row_checker, param + i);
        ret |= pthread_create(cols + i, NULL, &col_checker, param + i);
        ret |= pthread_create(blocks + i, NULL, &block_checker, param + i);
    }

    /*
    Collect threads
     */
    void *r, *c, *b; // for return values
    // type used for portability (int is 32 bit and void * is 64 bit)
    uintptr_t invalid = 0;
    for (i = 0; i < 9; i++)
    {
        ret = pthread_join(rows[i], &r);
        ret |= pthread_join(cols[i], &c);
        ret |= pthread_join(blocks[i], &b);
        if (ret != 0)
        {
            printf("pthread_join error.\n");
            break;
        }
        // invalid |= (uintptr_t)r;
        invalid |= ((uintptr_t)r | (uintptr_t)c | (uintptr_t)b);

        // don't 'break' because free might
        // cause a race condition error
    }

    if (invalid)
        printf("Sudoku solution invalid.\n");
    else
        printf("Sudoku solution valid.\n");

    // free memory
    free(param);
    free(data);
    return 0;
}

void *row_checker(void *arg)
{
    /*
    This function checks a row of the sudoku
    for duplicate values.
    return: 1 if duplicate found
            else 0
     */
    struct params *param = (struct params *)arg;

    // calculate offset
    int  offset = 9 * param->loc;
    int *base_ptr = param->data + offset;

    // array to check if value already exists
    int present[9] = {0}; // zero all elements

    int i;
    int curr_val;
    for (i = 0; i < 9; i++)
    {
        // -1 to be valid index in present
        curr_val = base_ptr[i] - 1;

        if (present[curr_val] != 1)
            present[curr_val] = 1;
        else
            return (void *)1; // duplicate number
    }
    return (void *)0; // no errors
}

void *col_checker(void *arg)
{
    /*
    This function checks a col of the sudoku
    for duplicate values.
    return: 1 if duplicate found
            else 0
     */
    struct params *param = (struct params *)arg;

    // calculate offset
    int  offset = param->loc;
    int *base_ptr = param->data + offset;

    // array to check if value already exists
    int present[9] = {0}; // zero all elements

    int i;
    int curr_val;
    for (i = 0; i < 9; i++)
    {
        // -1 to be valid index in present
        curr_val = base_ptr[9 * i] - 1;

        if (present[curr_val] != 1)
            present[curr_val] = 1;
        else
            return (void *)1; // duplicate number
    }
    return (void *)0; // no errors
}

void *block_checker(void *arg)
{
    /*
    This function checks a block of the sudoku
    for duplicate values.
    return: 1 if duplicate found
            else 0
     */
    struct params *param = (struct params *)arg;

    // calculate offset
    // exploit integer division
    int  offset = (param->loc / 3) * 27 + (param->loc % 3) * 3;
    int *base_ptr = param->data + offset;

    // array to check if value already exists
    int present[9] = {0}; // zero all elements

    int i, j;
    int curr_val;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            // -1 to be valid index in present
            curr_val = base_ptr[9 * i + j] - 1;

            if (present[curr_val] != 1)
                present[curr_val] = 1;
            else
                return (void *)1; // duplicate number
        }
    }
    return (void *)0; // no errors
}
