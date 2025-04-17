#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// main loop
int main()
{
	system("clear");
	int isRunning = 1; // Running condition for command line

	printf("<CTRL+X to end execution>\n");	  // Initial prompt
	printf("<CTRL+B to exit the shell>\n\n"); // Initial prompt
	char str[100];							  // Allocate space for input string
	char currDirectory[100];
	while (isRunning)
	{
		printf("citroShell$ ");			// Command line prompt
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
			if (command[0] == 'c' && command[1] == 'd')
			{
				char *temp = strtok(command, " ");
				temp = strtok(NULL, " ");
				chdir(temp);
				break;
			}

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