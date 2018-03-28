#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{

	if (argc != 3)
	{
		printf("loader <secured binary> <public key>\n");
		exit(0);
	}

	char *binary = argv[1];
	char *public_key = argv[2];

	// get the .sha section from binary
	pid_t pid_dump = fork();
	if (pid_dump == 0)
	{
		char *argv_objdump[] = {"objcopy", "--dump-section", ".sha=loader_temp.txt",
					  			binary, NULL};
		execvp(argv_objdump[0], argv_objdump);
		return -1;
	}

	// remove .sha section from binary
	pid_t pid_remove = fork();
	if (pid_remove == 0)
	{
		char *argv_objremove[] = {"objcopy", "--remove-section", ".sha",
								  binary, "loader-binary", NULL};
		execvp(argv_objremove[0], argv_objremove);
		return -1;
	}

	// wait for dump and remove to complete
	int status_dmp, status_rm;
	waitpid(pid_dump, &status_dmp, 0);
	waitpid(pid_remove, &status_rm, 0);

	// objcopy does not provide  useful return values
	// check that separating key from binary was successful
	/*if (status_dmp != 0 || status_rm != 0)
	{
		printf("Unsecured Binary\n");
		exit(-1);
	}*/

	// digest
	pid_t pid_dgst = fork();
	if (pid_dgst == 0)
	{
		char *argv_objdgst[] = {"openssl", "dgst", "-sha256", "-verify",
								public_key, "-signature", "loader_temp.txt",
								"loader-binary", NULL};
		execvp(argv_objdgst[0], argv_objdgst);
		return -1;
	}

	// wait for dgst to complete
	int status=0;
	waitpid(pid_dgst, &status, 0);

	// check that dgst was successful
	if (status != 0)
	{
		printf("Keys don't match\n");

		// clean up temp files
		system("rm loader-binary loader_temp.txt");
		exit(-1);
	}

	// format string for "system"
	size_t len = strlen(binary);
	char program[len+3];
	program[0] = '.';
	program[1] = '/';
	strncpy(program+2, binary, len+1);

	// run program
	system(program);

	// clean up temp files
	system("rm loader-binary loader_temp.txt");

	return 0;
}
