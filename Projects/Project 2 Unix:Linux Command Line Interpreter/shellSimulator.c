#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// we have to use execv() and fork() to start a child process 
// we need support for multiple simultaneous commands separated by semicolons
// ^ this means use wait() and waitpid()

// argc is the argument count, if they give 1 then a batch file is provided
int main(int argc, char *argv[]){

    if(argc == 2)   // batch file
    {
        // Get the filename (path to the batch file)
        char *batch_file = argv[1];

        // Open the batch file for reading
        FILE *file = fopen(batch_file, "r");
        char str[100];

        while (fgets(str, sizeof(str), file)) {
            str[strcspn(str, "\n")] = 0;    // Remove newline character

            // Skip empty lines and comments
            if (str[0] == '\0' || str[0] == '#') {
                continue;
            }

            char commandECHO[100];
            strcpy(commandECHO, "echo ");
            strcat(commandECHO, str);

            char* args[] = {"/bin/bash", "-c", commandECHO, NULL};  // Runs current command using echo
            execv("/bin/bash",args);    // Executes current command

            char* args2[] = {"/bin/bash", "-c", str, NULL};  // Actually runs the command
            execv("/bin/bash",args2);  // Executes current command  
        }

        fclose(file);
    }
    else    // no batch file
    {
        char str[100]; // allocate space for input string
        while(1){
            fgets(str, sizeof(str), stdin); // Read input, including spaces
            str[strcspn(str, "\n")] = 0;    // Remove newline character

            // splits up the input into semicolon-separated tokens
            char* command = strtok(str, ";"); 

            while(command != NULL)
            {

                char* args[] = {"/bin/bash", "-c", command, NULL};

                pid_t pid = fork(); // create a child process

                if(pid == 0){
                    if (execv("/bin/bash",args) == -1)
                    {
                        perror("exec failed");
                        exit(1);
                    }
                }
                else if (pid > 0){

                    int status;
                    pid_t childPID = waitpid(pid, &status, 0);
                }
                command = strtok(NULL, ";");
                
            }

        }
    }
    // Get the filename (path to the batch file)
    char *batch_file = argv[1];

    // Open the batch file for reading
    FILE *file = fopen(batch_file, "r");
    

    return 0;
}