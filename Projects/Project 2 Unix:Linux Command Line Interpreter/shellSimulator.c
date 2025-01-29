#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main()
{

    enum Commands
    {
        LS,
        CD,
        FIND
    };

    char str[10];
    char word1[10];
    char word2[10];
    int j = 0; // whitespace detection index

    printf("Enter a command: ");
    fgets(str, sizeof(str), stdin); // Read input, including spaces
    str[strcspn(str, "\n")] = 0;    // Remove newline character

    word1[0] = '\0';
    word2[0] = '\0';

    printf("%i", (int)(sizeof(str) / sizeof(char)));

    for (int i = 0; i < (int)(sizeof(str) / sizeof(char)); i++)
    {
        if ((int)str[i] == 32)
        {
            j = i;
            break;
        }
        word1[i] = str[i];
    }
    for (j = j + 1; j < (int)(sizeof(str) / sizeof(char)); j++)
    {
        word2[j] = str[j];
    }

    printf("%s\n", word1);
    printf("%s\n", word2);

    if (strcmp(str, "ls") == 0)
    {
        system("ls -l");
    }
    else if (strcmp(str, "cd") == 0)
    {
        system("cd test");
    }

    return 0;
}
