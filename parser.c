#include "shell.h"

void parse_input(char *input, Command *cmd) {
    int i = 0;
    char *token = strtok(input, " \t");
    while (token != NULL && i < MAX_ARGS - 1) {
        cmd->args[i++] = token;
        token = strtok(NULL, " \t");
    }
    cmd->args[i]    = NULL;
    cmd->arg_count  = i;
    cmd->is_background = 0;
    cmd->input_file    = NULL;
    cmd->output_file   = NULL;
}
