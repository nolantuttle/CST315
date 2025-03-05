#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

	FILE *fptr;

	char stringRead[50];

	fptr = fopen("input.txt", "r");

	// unsuccessful file read condition
	if (fptr == NULL)
	{
		printf("The file did not open.");

		exit(0);
	}
	else // file was read successfully
	{
		while (fgets(stringRead, 50, fptr) != NULL)
		{
			printf("%s", stringRead);
		}

		printf("\n");

		fclose(fptr);

		printf("The file is closed \n");
	}

	return 0;
}