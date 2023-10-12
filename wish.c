#include "wish.h"
#include <ctype.h>  // isspace
#include <regex.h>  // regcomp, regexec, regfree
#include <stdio.h>  // fopen, fclose, fileno, getline, feof
#include <stdlib.h> // exit
#include <sys/types.h>
#include <sys/wait.h> // waitpid

// Function to parse and run bash commands
int execute(char *buffer)
{
    char *path = "/usr/bin/bash"; // path to bash
    char *args[50];               // args in command
    char *token;                  // one word
    int i = 0;

    // Parse the command into arguments
    while ((token = strsep(&buffer, " ")) != NULL)
    {
        if (strlen(token) > 0) // if token not null
        {
            args[i++] = token;
        }
        // printf("%s\n",token);
    }
    args[i] = NULL; // Null-terminate the argument array

    // create child process
    pid_t pid = fork();

    if (pid == -1)
    {
        printf("Fork Failed");
        return 1;
    }
    else if (pid == 0)
    {
        execv(path, args);
        perror("execv");
    }
}

int main(int argc, char *argv[])
{
    char *buffer = NULL;
    size_t len = 0;
    int line;

    //  BATCH MODE
    if (argc > 1)
    {
        FILE *file; // file ptr

        if ((file = fopen(argv[1], "r")) == NULL) // open file
        {
            fprintf(stderr, "Input file (%s) not found.\n", argv[1]); // !!! file cannot be opened
            exit(1);
        }

        while ((line = getline(&buffer, &len, file)) != -1) // read file linewise
        {
            if (buffer[line - 1] == '\n') // Remove the trailing newline character
                buffer[line - 1] = '\0';

            printf("%s\n", buffer);
        }

        fclose(file);
        free(buffer);
        exit(0); // EOF
    }

    // INTERACTIVE MODE
    while (1)
    {
        printf("wish> ");

        // read line
        line = getline(&buffer, &len, stdin);

        // no comand entered
        if (line == -1)
            continue;

        // Remove the trailing newline character
        if (buffer[line - 1] == '\n')
            buffer[line - 1] = '\0';

        // break loop on "exit"
        if (strcasecmp(buffer, "exit") == 0)
        {
            printf("EXIT");
            free(buffer);
            exit(0); // EOF
        }
        printf("%s\n", buffer);
        execute(buffer);
    }
    return 0;
}