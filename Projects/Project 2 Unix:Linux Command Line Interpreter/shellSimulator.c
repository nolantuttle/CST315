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

    char str[100];
    char word1[10];
    char word2[10];
    int j = 0; // whitespace detection index

    printf("Enter a command: ");
    fgets(str, sizeof(str), stdin); // Read input, including spaces
    str[strcspn(str, "\n")] = 0;    // Remove newline character

    printf("%s\n", str);

    for (int i = 0; i < sizeof(str) / sizeof(str[0]); i++)
    {
        if (isspace(str[i]))
        {
            j = i;
            break;
        }
        else
        {
            word1[i] = str[i];
        }
    }
    for (j = j + 1; j < sizeof(str) / sizeof(str[0]); j++)
    {
        if (isspace(str[j]))
        {
            break;
        }
        else
        {
            word2[j] = str[j];
        }
    }

    printf("%s\n", word1);
    printf("%s", word2);

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
