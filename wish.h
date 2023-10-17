#include <pthread.h> // pthread_create, pthread_join
#include <regex.h>   // regcomp, regexec, regfree
#include <stdio.h>   // fopen, fclose, fileno, getline, feof
#include <string.h>  // strlen, strsep, strcat, strdup, strcmp
#include <unistd.h>  // STDERR_FILENO, fork, exec, access, exit, chdir

#ifdef REG_ENHANCED // macOS: man re_format
#define REG_CFLAGS REG_EXTENDED | REG_NOSUB | REG_ENHANCED
#else
#define REG_CFLAGS REG_EXTENDED | REG_NOSUB
#endif

#define INTERACTIVE_MODE 1
#define BATCH_MODE 2
#define BUFF_SIZE 256
#define MAX_ARGS 50

struct function_args
{
  pthread_t thread;
  char *command;
};

// Standard error message
void printError()
{
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

void print_error(char *msg);
void parse_line(char *line, char **arg);
void execute_path(char **arg);
void execute_cd(char **arg);
void execute_cmd(char **arg);
void reduce_arg(char **arg);
void del(char **arg);
int is_built_int(char *cmd);
int check_redirection(char **arg);

