#include "shell.h"

// ─── Global history storage ───────────────────────────────
char *history[MAX_HISTORY];
int   history_count = 0;

// ─── Main Shell Loop ──────────────────────────────────────
int main(void) {
    char    input[MAX_INPUT];
    Command cmd;

    // Set up signal handling (Ctrl+C won't kill the shell)
    setup_signals();

    while (1) {
        // 1. Print prompt
        printf("myShell> ");
        fflush(stdout);

        // 2. Read a line of input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // Ctrl+D pressed — exit gracefully
            printf("\n");
            break;
        }

        // 3. Remove trailing newline
        input[strcspn(input, "\n")] = '\0';

        // 4. Skip empty input
        if (strlen(input) == 0) continue;

        // 5. Save to history
        add_to_history(input);

        // 6. If pipe detected, hand off to pipe handler
        if (strchr(input, '|') != NULL) {
            execute_pipe(input);
            continue;
        }

        // 7. Parse input into Command struct
        memset(&cmd, 0, sizeof(Command));
        parse_input(input, &cmd);

        // 8. Skip if nothing was parsed
        if (cmd.arg_count == 0) continue;

        // 9. Built-in or external?
        if (is_builtin(cmd.args[0])) {
            run_builtin(&cmd);
        } else {
            execute_command(&cmd);
        }
    }

    return 0;
}
