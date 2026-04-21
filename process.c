#include "shell.h"

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
        // Child process

        // Restore/ignore signal based on execution mode
        if (cmd->is_background) {
            signal(SIGINT, SIG_IGN);  // background jobs ignore Ctrl+C
        } else {
            signal(SIGINT, SIG_DFL);  // foreground jobs respond normally
        }

        int fd;

        // Input redirection
        if (cmd->input_file != NULL) {
            fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0) {
                perror("input redirection");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // Output redirection
        if (cmd->output_file != NULL) {
            fd = open(cmd->output_file,
                      O_WRONLY | O_CREAT | O_TRUNC,
                      0644);
            if (fd < 0) {
                perror("output redirection");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // Execute command
        if (execvp(cmd->args[0], cmd->args) == -1) {
            perror(cmd->args[0]);  // accurate OS error instead of hardcoded message
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // Parent process

        if (cmd->is_background) {
            printf("[Background] PID: %d\n", pid);
        } 
        else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}
