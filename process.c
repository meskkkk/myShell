#include "shell.h"
#include <signal.h>

void execute_command(Command *cmd) {
    if (cmd == NULL || cmd->arg_count == 0) {
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        if (!cmd->is_background) {
            signal(SIGINT, SIG_DFL);
        }

        if (cmd->input_file != NULL || cmd->output_file != NULL) {
            setup_redirection(cmd);
        }

        if (execvp(cmd->args[0], cmd->args) == -1) {
            fprintf(stderr, "myShell: %s: command not found\n", cmd->args[0]);
            exit(EXIT_FAILURE);
        }
    } 
    else {
        if (cmd->is_background) {
            printf("[Background] PID: %d\n", pid);
        } 
        else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}
