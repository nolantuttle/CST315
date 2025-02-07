#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// argc is the argument count, if they give 1 then a batch file is provided
int main(int argc, char *argv[])
{
	int isRunning = 1; // Running condition for command line
	if (argc == 2)	   // Condition for batch file usage
	{
		// Get the filepath to batch file from argument 1
		char *batch_file = argv[1];

		// Open the batch file
		FILE *file = fopen(batch_file, "r");
		char str[100]; // Allocate space for input string

		while (fgets(str, sizeof(str), file))
		{
			str[strcspn(str, "\n")] = 0; // Remove newline character

			// Skip empty lines/comments
			if (str[0] == '\0' || str[0] == '#')
			{
				continue;
			}

			system(str); // Runs current line being read in batch file
		}

		fclose(file); // Close the batch file
	}

	printf("<CTRL+X to end execution>\n", NULL);  // Initial prompt
	printf("<CTRL+B to exit the shell>\n", NULL); // Initial prompt
	char str[100];								  // Allocate space for input string
	while (isRunning)
	{
		printf("seaShell$ ", NULL);		// Command line prompt
		fgets(str, sizeof(str), stdin); // Read input, including spaces
		str[strcspn(str, "\n")] = 0;	// Remove newline character

		// Condition for user inputting "CTRL+X" or the "quit" command
		if (str[0] == 24 || (!strcmp(str, "quit")))
		{
			exit(1); // Ends execution
		}
		// Condition for user inputting "CTRL+B" or the "exit" command
		else if (str[0] == 2 || (!strcmp(str, "exit")))
		{
			isRunning = 0;
			break;
		}

		// splits up the input into semicolon-separated tokens
		char *command = strtok(str, ";");

		while (command != NULL)
		{
			char *args[] = {"/bin/bash", "-c", command, NULL};

			pid_t pid = fork(); // create a child process

			if (pid == 0)
			{
				if (execv("/bin/bash", args) == -1)
				{
					perror("exec failed");
					exit(1);
				}
			}
			else if (pid > 0)
			{

				int status;
				pid_t childPID = waitpid(pid, &status, 0);
			}
			command = strtok(NULL, ";");
		}
	}

	return 0;
}