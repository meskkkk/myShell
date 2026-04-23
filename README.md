# myShell

A lightweight command-line shell built in C, developed as a final project for the
Operating Systems course at the Faculty of Computers and Data Science, Alexandria University.

---

## Team

| Name | ID | Contribution |
|------|----|--------------|
| Mesk Khaled | 23011537 | `myShell.c`, `shell.h`, `Makefile` |
| Basmala Moataz | 23012111 | `parser.c` |
| Shahd Fawzy | 23012087 | `process.c` |
| Adham Ashraf | 23011220 | `builtin.c` |
| Ziad Yasser | 23011277 | `io_signals.c` |

---

## Getting Started

```bash
make        # Compile
./myShell   # Run
make clean  # Remove build files
```

---

## Features

- [x] Interactive prompt (`myShell>`)
- [x] Command parsing with arguments
- [x] Built-in commands: `cd`, `pwd`, `exit`, `history`
- [x] Foreground execution
- [x] Background execution with `&` — prints PID immediately
- [x] Input redirection `<`
- [x] Output redirection `>`
- [x] Pipes `|` — supports multi-command pipelines
- [x] Signal handling — `Ctrl+C` kills foreground child only, shell stays alive
- [x] Error handling — command not found, bad paths, fork/pipe failures

---

## Project Structure

```
SP08/
├── myShell.c          # Core shell loop & integration
├── parser.c        # Command parsing & tokenization
├── process.c       # Process execution & background jobs
├── builtin.c       # Built-in commands & history
├── io_signals.c    # Redirection, pipes & signal handling
├── shell.h         # Shared headers, structs & prototypes
├── Makefile        # Build configuration
└── README.md       # This file
```

---

## Module Breakdown

### `myShell.c` — Core Shell Loop
Entry point of the shell. Displays the `myShell>` prompt, reads user input
in a loop, and coordinates calls to the parser and execution modules.
Handles `Ctrl+D` (EOF) for graceful exit.

---

### `parser.c` — Command Parsing & Tokenization
Takes raw input and breaks it into structured data for the rest of the shell.

- Splits input into tokens using space/tab delimiters
- Fills the `args[]` array passed to `execvp()`
- Detects and flags `&`, `>`, `<`
- Extracts filenames for redirection operators
- Handles empty input, extra spaces, and newlines gracefully

Populates a shared `Command` struct defined in `shell.h`.

---

### `process.c` — Process Execution & Background Jobs
Responsible for executing external commands by creating and managing processes.

- Uses `fork()` to create a child process per command
- Uses `execvp()` to run the requested program
- Handles input/output redirection directly inside the child using `dup2()`
- Foreground mode: parent waits for child via `waitpid()`
- Background mode: shell skips waiting and prints the process PID
- Foreground children restore `SIGINT` to default so `Ctrl+C` kills them
- Background children ignore `SIGINT` so `Ctrl+C` does not affect them

---

### `builtin.c` — Built-in Commands
Implements commands handled directly by the shell without forking.

- `cd` — changes directory; supports `~`, `..`, absolute and relative paths
- `pwd` — prints the current working directory using `getcwd()`
- `exit` — exits the shell with a goodbye message
- `history` — displays all commands typed this session, numbered
  - Stores up to 100 commands
  - Skips consecutive duplicate entries
  - Drops oldest entry automatically when full

---

### `io_signals.c` — Redirection, Pipes & Signal Handling
Manages I/O redirection, multi-command pipelines, and signal behavior.

- `setup_signals()` — shell ignores `SIGINT`; children restore default handling
- `execute_with_redirection()` — applies `<` and `>` using `open()` and `dup2()`
- `execute_pipe()` — tokenizes input on `|`, forks one child per segment,
  wires them together using `pipe()` and `dup2()` in a loop; supports
  pipelines of any length; parent waits for all children to finish

---

## Usage Examples

```bash
# Basic command
myShell> ls -la

# Built-ins
myShell> cd /tmp
myShell> pwd
myShell> history
myShell> exit

# Background execution
myShell> sleep 5 &
[Background] PID: 1234

# Output redirection
myShell> ls > output.txt

# Input redirection
myShell> cat < output.txt

# Single pipe
myShell> ls | grep .c

# Multi-command pipeline
myShell> ls | grep .c | wc -l

# Signal handling
myShell> sleep 10
^C                        # sleep is killed, shell survives

# Error handling
myShell> fakecmd
fakecmd: No such file or directory
myShell> cd /nonexistent
cd: No such file or directory
```

---

## Important Rules Followed

- `system()` is not used anywhere
- All external commands use `fork()` + `execvp()` + `wait()`/`waitpid()`
- No plagiarism — all logic implemented independently by team members

---
