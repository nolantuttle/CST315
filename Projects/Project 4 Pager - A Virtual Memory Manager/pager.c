#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PAGE_COUNT 8
#define LOGICAL_MEM_ROWS 4
#define LOGICAL_MEM_COLS 4
#define MAX_LINE_LENGTH (LOGICAL_MEM_COLS + 1)  // extra space for null terminator

// allocate extra space for null character
char logical_mem[LOGICAL_MEM_ROWS][MAX_LINE_LENGTH] = {"", "", "", ""};
char physical_mem[8][MAX_LINE_LENGTH];

int main()
{
    int page_table[PAGE_COUNT]= {4, 7, 2, 0};
    FILE *fptr;

    char stringRead[MAX_LINE_LENGTH];

    fptr = fopen("input.txt", "r");

    // unsuccessful file read condition
    if (fptr == NULL)
    {
        printf("The file did not open.");
        exit(0);
    }
    else // file was read successfully
    {
        int x = 0;
        while (fgets(stringRead, MAX_LINE_LENGTH, fptr) != NULL)
        {
            // Remove newline if present
            stringRead[strcspn(stringRead, "\n")] = 0;
            
            // Ensure we don't overflow logical_mem
            if (x >= LOGICAL_MEM_ROWS) {
                break;
            }
            
            // Copy up to LOGICAL_MEM_COLS characters
            strncpy(logical_mem[x], stringRead, LOGICAL_MEM_COLS);
            logical_mem[x][LOGICAL_MEM_COLS] = '\0';  // Ensure null termination
            x += 1;
        }
        for (int i = 0; i < LOGICAL_MEM_ROWS; i++) {
            printf("Logical Memory [%d]: %s\n", i, logical_mem[i]);
        }

        printf("\n");

        fclose(fptr);

        printf("The file is closed \n");
    }
    return 0;
}