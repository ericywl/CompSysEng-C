#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 8192

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void main() {//start main

    char command[MAX_INPUT];

    while (1) {
        printf("csh>");
        // take input from user
        fgets(command, MAX_INPUT, stdin);
        printf("command %s\n", command);

/*TO DO LIST:*/
/*
////----------------Case 1, create the external process and execute the command in that process----------------------

////----------------Case 2, change directory----------------------
//starts with cd

////--------------Case3, History-------------------------
//check if user enteres history option
*/

    }
}

#pragma clang diagnostic pop