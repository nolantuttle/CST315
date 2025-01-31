#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// function to concatenate two strings together with a space in the middle
char *concat(const char *s1, const char *s2){
    char *result = malloc(strlen(s1) + strlen(s2) + 2); // add 2 so there is room
    // for the null terminator character and space
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

// we have to use execv() and fork() to start a child process 
// we need support for multiple simultaneous commands separated by semicolons
// ^ this means use wait() and waitpid()

// mannnnnnnnnnnnnn

int main(){
    char str[100]; // allocate space for input string
    char *word1;   // first word before whitespace
    char *word2;   // second word (Everything after first word)
    pid_t pid;  // child process variable

    while(1)
    {

        fgets(str, sizeof(str), stdin); // Read input, including spaces
        str[strcspn(str, "\n")] = 0;    // Remove newline character

        word1 = strtok(str, " "); // fetches first word, space delimiter
        word2 = strtok(NULL, ""); // fetches second word, no delimiter

        // if there is no second word, initializes it to an empty string
        if (word2 == NULL){
            word2 = "";
        }

        //system(concat(word1, word2)); // concatenates the two words

        char* args[] = {"/bin/bash", "-c", concat(word1, word2), NULL};

        pid = fork(); // create a child process

        if(pid == 0){
            if (execv("/bin/bash",args) == -1)
            {
                perror("exec failed");
                return 1;
            }
            exit(1);
        }
        else if (pid >0){
            wait(NULL);
        }
    }

    return 0;
}