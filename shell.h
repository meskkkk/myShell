#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_INPUT     1024
#define MAX_ARGS      64
#define MAX_HISTORY   100

typedef struct {
    char *args[MAX_ARGS];
    int   arg_count;
    int   is_background;
    char *input_file;
    char *output_file;
} Command;

extern char *history[MAX_HISTORY];
extern int   history_count;

void parse_input(char *input, Command *cmd);

int  is_builtin(char *cmd_name);
void run_builtin(Command *cmd);
void add_to_history(char *input);

void execute_command(Command *cmd);

void setup_signals(void);
void execute_with_redirection(Command *cmd);
void execute_pipe(char *input);

#endif
