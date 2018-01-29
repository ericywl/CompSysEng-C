#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 8192
#define ARRAY_SIZE 1024
#define BUFSIZE 256
#define HISTORY_LIMIT 20

typedef char string[MAX_INPUT];

int exec_cmd(char command_line[MAX_INPUT]) {
    char buffer1[BUFSIZE];
    FILE *fp;

    if ((fp = popen(command_line, "r")) == NULL) {
        printf("Error opening pipe.\n");
        return -1;
    }

    while (fgets(buffer1, BUFSIZE, fp) != NULL) {
        printf("%s", buffer1);
    }

    if (pclose(fp)) {
        // printf("Command not found or exited with error status.\n");
        return -1;
    }

    return 0;
}

char *get_new_dir(char *cl_copy) {
    char temp_path[MAX_INPUT], temp[3];
    char *path, *param;
    size_t len;

    param = strsep(&cl_copy, " \n");
    if (strcmp(param, "") == 0) {
        return strdup(getenv("HOME"));
    }

    // copy param[0:1] to temp
    strncpy(temp, param, 2);
    temp[2] = '\0';
    // parent directory already handled by chdir
    if (strcmp(temp, "..") == 0) {
        return param;
    }

    // clear temp_path char array
    memset(temp_path, 0, strlen(temp_path));

    // copy param[1:] to temp_path
    if ((len = strlen(param)) != 1) {
        strncpy(temp_path, param + 1, len);
        temp_path[len - 1] = '\0';
    }

    // look at temp[0], which is param[0];
    temp[1] = '\0';
    if (strcmp(temp, "~") == 0) {
        path = strdup(getenv("HOME"));
        strcat(path, temp_path);
    } else if (strcmp(temp, ".") == 0) {
        char cwd[1024];
        path = strdup(getcwd(cwd, sizeof(cwd)));
        strcat(path, temp_path);
    } else {
        path = param;
    }

    return path;
}

void insert_history(string *array, char str[MAX_INPUT]) {
    for (int j = HISTORY_LIMIT - 1; j > 0; j--) {
        memmove(&array[j], &array[j - 1], sizeof(*array));
    }
    // copy str to array[0]
    strncpy(array[0], str, MAX_INPUT);
}

void run_shell() {
    char command_line[MAX_INPUT];
    char *cl_copy, *cmd, *path;
    string *hist_list = calloc(HISTORY_LIMIT, sizeof(*hist_list));

    while (1) {
        printf("csh> ");
        // take input from user
        fgets(command_line, MAX_INPUT, stdin);
        cl_copy = strdup(command_line);
        cmd = strsep(&cl_copy, " \n");

        // change directory
        if (strcmp(cmd, "cd") == 0) {
            path = get_new_dir(cl_copy);

            if (chdir(path) != 0) {
                char *param = strsep(&cl_copy, " \n");
                printf("%s does not exist.\n", param);
                continue;
            }

            insert_history(hist_list, command_line);
            continue;
        }

        // print out history
        if (strcmp(cmd, "history") == 0) {
            if (strcmp(hist_list[0], "") == 0) {
                printf("The history list is empty.\n");
                continue;
            }

            for (int i = 0; i < HISTORY_LIMIT; i++) {
                if (strcmp(hist_list[i], "") != 0)
                    printf("%6d  %s", i + 1, hist_list[i]);
            }
            continue;
        }

        // execute previous command
        if (strcmp(cmd, "!!") == 0) {
            if (strcmp(hist_list[0], "") == 0) {
                printf("The history list is empty.\n");
                continue;
            }

            memset(cl_copy, 0, strlen(cl_copy));
            strncpy(cl_copy, hist_list[0], strlen(hist_list[0]));
            exec_cmd(cl_copy);
            continue;
        }

        // execute indexed command
        int index = atoi(command_line);
        if (index != 0) {
            if (strcmp(hist_list[0], "") == 0) {
                printf("The history list is empty.\n");
                continue;
            }

            if (index >= HISTORY_LIMIT || index < 1) {
                printf("The index is not in range 1-%d.\n", HISTORY_LIMIT - 1);
                continue;
            }

            if (strcmp(hist_list[index - 1], "") == 0) {
                printf("There is no command with that index.\n");
                continue;
            }

            exec_cmd(hist_list[index - 1]);
            continue;
        }

        // if the command is not all of the above, execute it using system
        if (exec_cmd(command_line) != -1) {
            insert_history(hist_list, command_line);
        }
    }
}

int main() {
    run_shell();
    return 0;
}