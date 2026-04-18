#include "shell.h"

void execute_command(Command *cmd) {
    if (cmd == NULL || cmd->arg_count == 0){
       return;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    else if (pid == 0) {
        if (cmd->input_file != NULL || cmd->output_file != NULL) {
           execute_with_redirection(cmd);
           exit(0);
        }
        else {
             if (execvp(cmd->args[0],cmd->args) == -1) {
                fprintf(stderr,"myShell: %s: command not found\n",cmd->args[0]);
                exit(EXIT_FAILURE);
             }
        }
    }
    else {
        if(cmd->is_background) {
           printf ("[Background] PID: %d\n", pid);
        }
        else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

