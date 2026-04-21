#include "shell.h"
#include <errno.h>

// ─── Signal handler for Ctrl+C ────────────────────────────
void setup_signals(void) {
    // Shell ignores Ctrl+C — children reset to default so they can be killed
    signal(SIGINT, SIG_IGN);
}

// ─── Execute command with redirection (kept for compatibility) ────
void execute_with_redirection(Command *cmd) {
    (void)cmd; // redirection is handled directly in process.c
}

// ─── Execute pipe command (cmd1 | cmd2) ───────────────────
void execute_pipe(char *input) {
    char *cmd1_str, *cmd2_str;
    Command cmd1, cmd2;
    int pipefd[2];
    pid_t pid1, pid2;

    // Split input at the pipe character
    cmd1_str = strtok(input, "|");
    cmd2_str = strtok(NULL, "|");

    if (cmd2_str == NULL) {
        fprintf(stderr, "pipe usage: cmd1 | cmd2\n");
        return;
    }

    // Parse both commands
    memset(&cmd1, 0, sizeof(Command));
    memset(&cmd2, 0, sizeof(Command));
    parse_input(cmd1_str, &cmd1);
    parse_input(cmd2_str, &cmd2);

    // Validate both commands
    if (cmd1.arg_count == 0 || cmd2.arg_count == 0) {
        fprintf(stderr, "pipe: invalid command\n");
        return;
    }

    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return;
    }

    // First child — writes to pipe
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork failed");
        return;
    }
    if (pid1 == 0) {
        signal(SIGINT, SIG_DFL);        // child can be killed by Ctrl+C
        dup2(pipefd[1], STDOUT_FILENO); // stdout → pipe write end
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(cmd1.args[0], cmd1.args);
        perror("execvp failed");
        exit(1);
    }

    // Second child — reads from pipe
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork failed");
        return;
    }
    if (pid2 == 0) {
        signal(SIGINT, SIG_DFL);        // child can be killed by Ctrl+C
        dup2(pipefd[0], STDIN_FILENO);  // stdin → pipe read end
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(cmd2.args[0], cmd2.args);
        perror("execvp failed");
        exit(1);
    }

    // Parent — close pipe ends and wait for both children
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
