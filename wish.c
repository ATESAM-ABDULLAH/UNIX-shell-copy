#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64
#define MAX_PATHS 64

char* paths[MAX_PATHS]={0}; // Array to store paths
int numPaths = 0; // Number of paths in the array

// Function to print error message
void printError() {
    fprintf(stderr,"An error has occurred\n");
}

// Function to execute command or shell script
void executeCommand(char **args, int background) {
    int i;
    pid_t pid;
    int found = 0;
    char cmd[MAX_INPUT_SIZE];

    // Try to find the shell script in each path in the paths array
    for (i = 0; i < numPaths; i++) {
        // printf("path = %s\n",paths[i]);
        snprintf(cmd, sizeof(cmd), "%s/%s", paths[i], args[0]);
        // printf("cmd = %s\n",cmd);

        if (access(cmd, X_OK) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printError();
        return;
    }

    pid = fork();
    if (pid < 0) {
        printError();
    } else if (pid == 0) {
        // Child process
        execv(cmd, args);
        // If execv returns, it must have failed
        printError();
    } else {
        // Parent process
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

// Function to handle built-in commands
int handleBuiltInCommands(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) {
            printError();
        }
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            printError();
        } else {
            if (chdir(args[1]) != 0) {
                printError();
            }
        }
        return 1;
    } else if (strcmp(args[0], "path") == 0) {
        // Clear existing paths
        numPaths = 0;
        // Skip the "path" command itself and update paths with provided directories
        if (args[1] != NULL) {
            for (int i = 1; args[i] != NULL; i++) {
                paths[numPaths++] = args[i];
            }
        }
        // printf("paths %d\n",numPaths);

        return 1;
    }
    return 0;
}

// Function to parse input command
void parseInput(char *input) {
    char *token;
    char *args[MAX_ARGS];
    int background = 0;

    token = strtok(input, " \t\n");
    int i = 0;
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1;
            break;
        }
        args[i++] = token;
        // printf("arg[%d] = %s\n",i-1,args[i-1]);
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;

    if (args[0] == NULL) {
        return;
    }

    if (!handleBuiltInCommands(args)) {
        // Not a built-in command
        executeCommand(args, background);
    }
}

int main(int argc, char *argv[]) {
    char input[MAX_INPUT_SIZE];
    FILE *file = NULL;

    // Check if batch mode
    if (argc > 1) {
        file = fopen(argv[1], "r");
        if (file == NULL) {
            printError();
        }
    }

    numPaths++;
    paths[0] = "/bin"; // Initial directory

    while (1) {
        if (file == NULL) {
            printf("wish> ");
            if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
                break;
            } 
        } else {
            if (fgets(input, MAX_INPUT_SIZE, file) == NULL) {
                break;
            }
        }

        input[strcspn(input, "\n")] = '\0'; // Replace newline with NULL

        parseInput(input);
    }

    if (file != NULL) {
        fclose(file);
    }

    return 0;
}
