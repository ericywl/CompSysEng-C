#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 8192

char * get_new_dir(char *cl_copy) {
    char temp_path[MAX_INPUT], temp[3];
    char *path, *param;

    param = strsep(&cl_copy, " \n");

    if (strcmp(param, "") == 0)
        path = strdup(getenv("HOME"));
    else {
        // copy param[0:1] to temp
        strncpy(temp, param, 2);
        temp[2] = '\0';

        // parent directory already handled by chdir
        if (strcmp(temp, "..") == 0) {
            return param;
        }
        // look at temp[0], which is param[0];
        temp[1] = '\0';

        // clear temp_path char array
        memset(temp_path, 0, strlen(temp_path));
        size_t len;
        if ((len = strlen(param)) != 1) {
            // copy param[1:] to temp_path
            strncpy(temp_path, param + 1, len);
            temp_path[len - 1] = '\0';
        }

        if (strcmp(temp, "~") == 0) {
            // concat temp_path and home directory
            path = strdup(getenv("HOME"));
            strcat(path, temp_path);
        } else if (strcmp(temp, ".") == 0) {
            // concat temp_path and current directory
            char cwd[1024];
            path = strdup(getcwd(cwd, sizeof(cwd)));
            strcat(path, temp_path);
        } else {
            path = strdup(param);
        }
    }

    return path;
}

int main() {
    char command_line[MAX_INPUT];
    char *cl_copy, *cmd, *path;

    while (1) {
        printf("csh> ");
        // take input from user
        fgets(command_line, MAX_INPUT, stdin);
        cl_copy = strdup(command_line);
        cmd = strsep(&cl_copy, " \n");

        if (strcmp(cmd, "cd") == 0) {
            path = get_new_dir(cl_copy);
            chdir(path);
        } else {
            system(command_line);
        }

        ////--------------Case3, History-------------------------
        //check if user enters history option
    }

    return (0);
}