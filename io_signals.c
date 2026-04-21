#include "shell.h"

void setup_signals(void) {
    // Ignore SIGINT (Ctrl+C) in the parent shell process
    // Child processes will restore default behavior
    signal(SIGINT, SIG_IGN);
}

void execute_with_redirection(Command *cmd) {
    // Handle input redirection (< file)
    if (cmd->input_file != NULL) {
        int fd_in = open(cmd->input_file, O_RDONLY);
        if (fd_in == -1) {
            perror("redirection input error");
            exit(EXIT_FAILURE);
        }
        // Replace stdin with the file
        if (dup2(fd_in, STDIN_FILENO) == -1) {
            perror("dup2 input error");
            exit(EXIT_FAILURE);
        }
        close(fd_in);
    }
    
    // Handle output redirection (> file)
    if (cmd->output_file != NULL) {
        int fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1) {
            perror("redirection output error");
            exit(EXIT_FAILURE);
        }
        // Replace stdout with the file
        if (dup2(fd_out, STDOUT_FILENO) == -1) {
            perror("dup2 output error");
            exit(EXIT_FAILURE);
        }
        close(fd_out);
    }
}

void execute_pipe(char *input) {
    char *commands[MAX_ARGS];
    int cmd_count = 0;
    int pipefds[2];
    pid_t pid;
    int prev_fd = -1;
    int i;
    
    // Parse the input into separate commands based on pipe symbol
    char *token = strtok(input, "|");
    while (token != NULL && cmd_count < MAX_ARGS - 1) {
        // Skip leading/trailing whitespace
        while (*token == ' ' || *token == '\t') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';
        
        commands[cmd_count] = token;
        cmd_count++;
        token = strtok(NULL, "|");
    }
    commands[cmd_count] = NULL;
    
    if (cmd_count == 0) return;
    
    // Execute each command in the pipeline
    for (i = 0; i < cmd_count; i++) {
        // Create pipe for all except the last command
        if (i < cmd_count - 1) {
            if (pipe(pipefds) == -1) {
                perror("pipe");
                return;
            }
        }
        
        pid = fork();
        if (pid == -1) {
            perror("fork");
            return;
        }
        
        if (pid == 0) {
            // Child process
            
            // Restore default signal handling for child
            signal(SIGINT, SIG_DFL);
            
            // If not the first command, read from previous pipe
            if (prev_fd != -1) {
                if (dup2(prev_fd, STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(EXIT_FAILURE);
                }
                close(prev_fd);
            }
            
            // If not the last command, write to current pipe
            if (i < cmd_count - 1) {
                if (dup2(pipefds[1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(EXIT_FAILURE);
                }
                close(pipefds[0]);
                close(pipefds[1]);
            }
            
            // Parse and execute the command
            Command cmd_struct;
            memset(&cmd_struct, 0, sizeof(Command));
            
            // Create a copy of the command string since strtok modifies it
            char cmd_copy[MAX_INPUT];
            strncpy(cmd_copy, commands[i], MAX_INPUT - 1);
            cmd_copy[MAX_INPUT - 1] = '\0';
            
            parse_input(cmd_copy, &cmd_struct);
            
            // Check for redirection within the piped command
            if (cmd_struct.input_file != NULL || cmd_struct.output_file != NULL) {
                execute_with_redirection(&cmd_struct);
            }
            
            // Execute the command
            if (execvp(cmd_struct.args[0], cmd_struct.args) == -1) {
                fprintf(stderr, "myShell: %s: command not found\n", cmd_struct.args[0]);
                exit(EXIT_FAILURE);
            }
        }
        
        // Parent process
        // Close previous pipe read end if it exists
        if (prev_fd != -1) {
            close(prev_fd);
        }
        
        // Close pipe write end if not last command
        if (i < cmd_count - 1) {
            close(pipefds[1]);
            prev_fd = pipefds[0];
        } else {
            prev_fd = -1;
        }
    }
    
    // Wait for all child processes to complete
    for (i = 0; i < cmd_count; i++) {
        int status;
        wait(&status);
    }
}



