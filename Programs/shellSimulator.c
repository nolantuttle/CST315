#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    enum Commands
    {
        LS,
        CD,
        FIND
    };


        char str[3];

    printf("Enter a command: ");
    fgets(str, sizeof(str), stdin);  // Read input, including spaces
    str[strcspn(str, "\n")] = 0;  // Remove newline character
        if(strcmp(str, "ls")==0) {
            system("ls -l");
        }
        else if(strcmp(str, "cd")==0){
            
        }

    

    return 0;
}




