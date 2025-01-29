#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 2); // add 2 so there is room
    // for the null terminator character and space
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

int main()
{
    char str[100];
    char *word1;
    char *word2;

    printf("Enter a command: ");
    fgets(str, sizeof(str), stdin); // Read input, including spaces
    str[strcspn(str, "\n")] = 0;    // Remove newline character

    word1 = strtok(str, " ");
    word2 = strtok(NULL, "");

    if (word2 == NULL)
    {
        word2 = "";
    }

    if (strcmp(word1, "ls") == 0)
    {
        system(concat(word1, word2)); // concatenates the two words
    }
    else if (strcmp(word1, "cd") == 0)
    {
        system(concat(word1, word2)); // concatenates the two words
    }
    else if (strcmp(word1, "mkdir") == 0)
    {
        system(concat(word1, word2)); // concatenates the two words
    }
    else if (strcmp(word1, "pwd") == 0)
    {
        system(concat(word1, word2)); // concatenates the two words
    }
    else if (strcmp(word1, "echo") == 0)
    {
        system(concat(word1, word2)); // concatenates the two words
    }

    return 0;
}