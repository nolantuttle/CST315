#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

typedef struct
{
	char command[100];
	pid_t pid;
	int finished;
	clock_t arrivalTime;
	clock_t completionTime;
} Process;

Process processQueue[10]; // limit of 10 processes in the queue
int processCount = 0;

// Displays currently active processes and their states
void displayProcessStates(int currentIndex)
{
	printf("\n+----+----------------------------+----------+\n");
	printf("| ID | Command                    | Status   |\n");
	printf("+----+----------------------------+----------+\n");
	for (int i = 0; i < processCount; i++)
	{
		printf("| %2d | %-26s | ", i, processQueue[i].command);
		if (processQueue[i].finished)
		{
			printf("FINISHED |\n");
		}
		else if (i == currentIndex)
		{
			printf("RUNNING  |\n");
		}
		else
		{
			printf("BLOCKED  |\n");
		}
	}
	printf("+----+----------------------------+----------+\n\n");
}

void enqueueCommand(const char *cmd)
{
	if (processCount >= 10)
		return;
	strncpy(processQueue[processCount].command, cmd, sizeof(processQueue[processCount].command));
	processQueue[processCount].finished = 0; // Sets finished status to 0 (Unfinished process)
	processQueue[processCount].pid = -1;	 // Sets default process ID to -1
	processCount++;
	processQueue[processCount].arrivalTime = clock(); // Store the arrival time
	processQueue[processCount].completionTime = 0;	  // Sets completion time to 0 as default
}

/** Runs the command-line interpreter
 * argc is the argument count, if they give 1 then a batch file is provided
 **/
void runShell()
{
	int time_quantum = 2; // Default time quantum (in seconds)
	int isRunning = 1;	  // If false, exits interpreter

	// Random processes that will run when scheduler starts up
	char *idleCommands[] = {
		"sleep 2",
		"echo idle1 > /dev/null",
		"sleep 4"};
	int idleCount = sizeof(idleCommands) / sizeof(idleCommands[0]);

	system("clear");
	printf("<Welcome to our Short-Term Scheduler/Command Line Interpeter!>\n");				  // Initial prompt
	printf("<Running a command will add it to a Round-Robin queue with 3 idle processes>\n"); // Initial prompt
	printf("<USE 'setquantum (time in seconds)' TO ADJUST THE SCHEDULER'S TIME QUANTUM>\n");  // Initial prompt
	printf("<CURRENT QUANTUM: %i>\n", time_quantum);
	char str[100]; // Allocate space for input string
	char currDirectory[100];
	while (isRunning)
	{
		// Queues up idle processes to fill up the process table
		for (int i = 0; i < idleCount && processCount < 10; i++)
		{
			enqueueCommand(idleCommands[i]);
		}
		printf("seaShell$ ");			// Command line prompt
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
			// If command matches procs prints out basic info of processes using ps command, sorts by k2 which is column 2(user)
			if (strcmp(command, "procs") == 0)
			{
				system("ps -eo pid,user,stat,comm |sort -k2");
				return; // Ends program
			}
			// Prints out more advanced info and sorts by column 4(cpu percentage) in number reverse order (least to greatest)
			else if (strcmp(command, "procs -a") == 0)
			{
				system("ps -eo pid,ppid,user,%cpu,%mem,vsz,rss,tty,stat,start,time,command|sort -k4 -nr");
				return; // Ends program
			}
			// Same as above command but sorts numerically by process id
			else if (strcmp(command, "procs -a -si") == 0)
			{
				system("ps -eo pid,ppid,user,%cpu,%mem,vsz,rss,tty,stat,start,time,command");
				return; // Ends program
			}
			// Allows the user to set a custom time quantum
			else if (strncmp(command, "setquantum ", 11) == 0)
			{
				int new_quantum = atoi(command + 11);
				if (new_quantum > 0)
				{
					time_quantum = new_quantum;
					printf("Time quantum set to %d seconds.\n", time_quantum);
				}
				else
				{
					printf("Invalid time quantum.\n");
				}
				break;
			}
			else if (processCount < 10)
			{
				enqueueCommand(command);
			}

			command = strtok(NULL, ";"); // Tokenize inputs by semicolon
		}

		int finishedProcesses = 0;
		for (int i = 0; i < processCount; i++) // This loop queues up all processes and pauses them immediately to be run by the scheduler
		{
			if (strncmp(processQueue[i].command, "cd ", 3) == 0)
			{
				sleep(time_quantum); // Simulate time quantum used

				// Extract the path and change directory
				char *path = strtok(processQueue[i].command, " ");
				path = strtok(NULL, " "); // The argument for cd
				if (path)
					chdir(path);

				processQueue[i].finished = 1;
				finishedProcesses++;
				displayProcessStates(i);
				continue;
			}

			pid_t pid = fork(); // Forks a new process

			if (pid == 0) // Child process
			{
				char *args[] = {"/bin/bash", "-c", processQueue[i].command, NULL};
				if (execv("/bin/bash", args) == -1)
				{
					perror("exec failed");
					exit(1);
				}
			}
			else if (pid > 0) // Parent process
			{
				processQueue[i].pid = pid;
				kill(pid, SIGSTOP);
			}
		}

		while (finishedProcesses < processCount) // Scheduler loop that runs processes waiting in the queue
		{
			for (int i = 0; i < processCount; i++)
			{
				if (processQueue[i].finished)
					continue;

				kill(processQueue[i].pid, SIGCONT); // Signals current process to start running
				displayProcessStates(i);
				sleep(time_quantum);				// Runs only for time quantum
				kill(processQueue[i].pid, SIGSTOP); // Pauses the process

				int status;
				pid_t result = waitpid(processQueue[i].pid, &status, WNOHANG); // Checks if the current process is finished
				if (result == processQueue[i].pid)							   // If it has finished, sets condition and increments finished counter
				{
					processQueue[i].finished = 1;
					processQueue[i].completionTime = clock(); // Store the completion time when process finishes
					finishedProcesses++;
				}
			}
		}
		for (int i = 0; i < processCount; i++)
		{
			if (processQueue[i].finished)
			{
				double turnaroundTime = (double)(processQueue[i].completionTime - processQueue[i].arrivalTime) / CLOCKS_PER_SEC;
				printf("Process %d: Turnaround Time = %.4f seconds\n", i, turnaroundTime);
			}
		}
		for (int i = 0; i < processCount; i++)
		{
			waitpid(processQueue[i].pid, NULL, 0); // Ensure all processes are cleaned up
		}

		processCount = 0; // Reset process count
	}
}

int main(int argc, char *argv[])
{
	runShell();

	return 0;
}