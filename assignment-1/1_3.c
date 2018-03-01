#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <dirent.h>

int main()
{
    // fork child process to run commands
    int status;
    int pid = fork();
    if (pid == 0)
    {
        printf("Child start\n");
        // C std library calls
        struct dirent **curr_dir;

        int n = scandir(".", &curr_dir, NULL, alphasort);
        if (n < 0)
        {
            perror("scandir");
            return n;
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
        printf("Child Done\n");
    }
    else
    {
        printf("Hello from Parent\n");
        wait(&status); // wait for child
        printf("Parent Done\n");
    }
    return 0;
}
