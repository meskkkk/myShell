#include "shell.h"

// ─── Built-in command checker ─────────────────────────────
int is_builtin(char *name) {
    if (!name) return 0;
    return (strcmp(name, "cd")      == 0 ||
            strcmp(name, "exit")    == 0 ||
            strcmp(name, "pwd")     == 0 ||
            strcmp(name, "history") == 0);
}

// ─── Change directory (cd) ────────────────────────────────
static void builtin_cd(Command *cmd) {
    char *path = cmd->args[1];
    
    // If no argument or "~", go to home directory
    if (path == NULL || strcmp(path, "~") == 0) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
    }
    
    // Change directory
    if (chdir(path) != 0) {
        perror("cd");
    }
}

// ─── Print working directory (pwd) ────────────────────────
static void builtin_pwd(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

// ─── Exit shell ───────────────────────────────────────────
static void builtin_exit(void) {
    printf("Exiting myShell. Goodbye!\n");
    exit(0);
}

// ─── Display command history ──────────────────────────────
static void builtin_history(void) {
    printf("\nCommand History:\n");
    printf("----------------\n");
    
    if (history_count == 0) {
        printf("(no commands yet)\n");
    } else {
        int start = (history_count > MAX_HISTORY) ? history_count - MAX_HISTORY : 0;
        for (int i = start; i < history_count; i++) {
            printf("%4d  %s\n", i + 1, history[i]);
        }
    }
    printf("\n");
}

// ─── Main built-in dispatcher ─────────────────────────────
void run_builtin(Command *cmd) {
    if (strcmp(cmd->args[0], "cd") == 0) {
        builtin_cd(cmd);
    }
    else if (strcmp(cmd->args[0], "pwd") == 0) {
        builtin_pwd();
    }
    else if (strcmp(cmd->args[0], "exit") == 0) {
        builtin_exit();
    }
    else if (strcmp(cmd->args[0], "history") == 0) {
        builtin_history();
    }
}

// ─── Add command to history (with cleanup) ────────────────
void add_to_history(char *input) {
    // Don't add empty lines
    if (input == NULL || strlen(input) == 0) return;
    
    // Don't add duplicate of last command (optional but nice)
    if (history_count > 0 && strcmp(history[history_count - 1], input) == 0) {
        return;
    }
    
    // If history is full, free oldest and shift
    if (history_count >= MAX_HISTORY) {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history_count = MAX_HISTORY - 1;
    }
    
    // Add new command
    history[history_count++] = strdup(input);
}


