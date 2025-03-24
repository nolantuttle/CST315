#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PAGE_COUNT 4
#define PAGE_SIZE 4
#define LOGICAL_MEM_SIZE 4
#define PHYSICAL_MEM_SIZE 8
#define MAX_LINE_LENGTH (PAGE_SIZE + 1) // extra space for null terminator

// allocate extra space for null character
char logical_mem[LOGICAL_MEM_SIZE][MAX_LINE_LENGTH] = {"", "", "", ""};
char logical_address[2];
char physical_mem[PHYSICAL_MEM_SIZE][MAX_LINE_LENGTH];

int main()
{
    int page_table[LOGICAL_MEM_SIZE] = {4, 7, 2, 0}; // Mapping logical memory to page table frames
    FILE *fptr;
    FILE *out_fptr;

    char stringRead[MAX_LINE_LENGTH];

    fptr = fopen("input.txt", "r"); // Open input file (logical memory)

    // Unsuccessful file read condition
    if (fptr == NULL)
    {
        printf("The file did not open.");
        exit(0);
    }
    else // File read successfully
    {
        int x = 0;
        while (fgets(stringRead, MAX_LINE_LENGTH, fptr) != NULL) // Read 1 4-byte line
        {
            // Remove newline if present
            stringRead[strcspn(stringRead, "\n")] = 0;

            // Overflow logical memory condition
            if (x >= LOGICAL_MEM_SIZE)
            {
                break;
            }

            // Copy all characters in 4-byte string to logical memory
            strncpy(logical_mem[x], stringRead, PAGE_SIZE);
            logical_mem[x][PAGE_SIZE] = '\0'; // Ensure null termination
            x += 1;
        }
        fclose(fptr); // Close the input file
        printf("Logical memory has been stored.\n");
    }

    // Address Translation: Map logical memory to physical memory using the page table
    for (int i = 0; i < LOGICAL_MEM_SIZE; i++)
    {
        int frame = page_table[i]; // Get the physical frame for logical page i
        if (frame < PHYSICAL_MEM_SIZE)
        {
            strncpy(physical_mem[frame], logical_mem[i], PAGE_SIZE);
            physical_mem[frame][PAGE_SIZE] = '\0'; // Ensure null termination
        }
    }

    // Write the physical memory to output.txt
    out_fptr = fopen("output.txt", "w");

    // Unsuccessful file write condition
    if (out_fptr == NULL)
    {
        printf("Error opening output file.\n");
        exit(0);
    }

    // Writing to output.txt the frame numbers and the physical memory stored there
    // The order of the frames is as follows:
    // 1st 4 bytes: Frame 4
    // 2nd 4 bytes: Frame 7
    // 3rd 4 bytes: Frame 2
    // 4th 4 bytes: Frame 0
    for (int i = 0; i < PHYSICAL_MEM_SIZE; i++)
    {
        fprintf(out_fptr, "Frame %d: %s\n", i, physical_mem[i]);
    }

    // Close the output file being written to.
    fclose(out_fptr);
    printf("Physical memory written to output.txt\n");

    printf("All pages have successfully been mapped. Exiting the program. \n");

    return 0;
}