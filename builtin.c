#include "shell.h"

int is_builtin(char *name) {
    if (!name) return 0;
    return (strcmp(name, "cd")      == 0 ||
            strcmp(name, "exit")    == 0 ||
            strcmp(name, "pwd")     == 0 ||
            strcmp(name, "history") == 0);
}

void run_builtin(Command *cmd) {
    // Temporary stub — Member 4 replaces this
    printf("[builtin stub] command: %s\n", cmd->args[0]);
}

void add_to_history(char *input) {
    if (history_count < MAX_HISTORY)
        history[history_count++] = strdup(input);
}
