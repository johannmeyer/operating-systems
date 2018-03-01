#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>

int main()
{
    // fork child process to run commands
    system("ls");
    system("mkdir child");

    // C std library calls
    struct dirent **curr_dir;

    int n = scandir(".", &curr_dir, NULL, alphasort);
    if (n < 0)
        perror("scandir");
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

    int mode = 777;
    int err = mkdir("parent", mode);
    if (err)
        perror("mkdir: cannot create directory");

    printf("\n");

    return 0;
}
