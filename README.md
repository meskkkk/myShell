# myShell

A lightweight command-line shell built in C, developed as a final project for the Operating Systems course at the Faculty of Computers and Data Science, Alexandria University.

---

## Team

| Name | ID | Contribution |
|------|----|--------------|
| Mesk Khaled | 23011537 | `main.c`, `shell.h`, `Makefile`, integration |
| Basmala Moataz | 23012111 | `parser.c` |
| Shahd Fawzy | 23012087 | `process.c` |
| Adham Ashraf | 23011220 | `builtin.c` |
| Ziad Yasser | 23011277 | `io_signals.c` |

---

## Getting Started

```bash
make        # Compile
./myShell   # Run
```

---

## Features

- Interactive prompt (`myShell>`)
- Command parsing with arguments
- Built-in commands: `cd`, `pwd`, `exit`, `history`
- Foreground and background execution with `&` (prints PID)
- Input/output redirection (`<`, `>`)
- Pipes (`|`)
- Signal handling — `Ctrl+C` kills child process only, shell stays alive
- Error handling — command not found, bad paths, fork/pipe failures

---

## Project Structure

```
myshell/
├── main.c          # Core shell loop & integration
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

### `main.c` — Core Shell Loop
Entry point of the shell. Displays the `myShell>` prompt, reads user input in a loop, and coordinates calls to the parser and execution modules.

---

### `parser.c` — Command Parsing & Tokenization
Takes raw input and breaks it into structured data for the rest of the shell.

- **Tokenization** — Splits input into tokens using space/tab delimiters
- **Argument building** — Fills the `args[]` array passed to `execvp()`
- **Operator detection** — Flags `&`, `>`, `<`, and `|`
- **Filename extraction** — Captures target/source filenames for redirection
- **Edge case handling** — Handles empty input, extra spaces, and newlines gracefully

Fills a shared `Command` struct (defined in `shell.h`) that all other modules read from.

---

### `process.c` — Process Execution & Background Jobs
Responsible for executing external commands by creating and managing processes.

- **Process creation** — Uses `fork()` to create a child process per command
- **Command execution** — Uses `execvp()` to run the requested program
- **Foreground mode** — Parent waits for child via `waitpid()`
- **Background mode** — Shell skips waiting and prints the process PID
- **Error handling** — Handles `fork()` and `execvp()` failures with clear messages

---

### `builtin.c` — Built-in Commands
Implements commands handled directly by the shell without forking.

- **`cd`** — Changes directory; supports `~`, `..`, absolute and relative paths
- **`pwd`** — Prints the current working directory
- **`exit`** — Exits the shell with a goodbye message
- **`history`** — Displays all commands typed this session with line numbers
  - Stores up to 100 commands
  - Skips consecutive duplicate entries
  - Automatically drops oldest entries when full

---

### `io_signals.c` — Redirection, Pipes & Signal Handling
Manages I/O redirection, pipes between commands, and signal behavior for the shell process.

- **Signal setup** — Ignores `SIGINT` (`Ctrl+C`) in the parent shell; child processes restore default signal handling so they can be interrupted normally
- **Input redirection** — Opens the source file with `O_RDONLY` and replaces `stdin` using `dup2()`
- **Output redirection** — Opens or creates the target file with `O_WRONLY | O_CREAT | O_TRUNC` and replaces `stdout` using `dup2()`
- **Pipe execution** — Tokenizes the input on `|`, forks one child per command, and wires them together using `pipe()` and `dup2()`; the parent closes pipe ends incrementally and waits for all children to finish
- **Redirection within pipes** — If a piped command contains `<` or `>`, redirection is applied inside the child before `execvp()`

**Functions:**

| Function | Description |
|----------|-------------|
| `setup_signals()` | Configures `SIGINT` to be ignored by the parent shell |
| `execute_with_redirection(Command *cmd)` | Applies input/output redirection for a command |
| `execute_pipe(char *input)` | Parses, forks, and executes a pipeline of commands |

---

## Usage Examples

```bash
# Basic command
myShell> ls -la

# Background execution
myShell> sleep 5 &
[PID: 1234]

# Output redirection
myShell> ls > output.txt

# Input redirection
myShell> cat < input.txt

# Pipe
myShell> ls | grep .c

# Built-ins
myShell> cd /tmp
myShell> pwd
myShell> history
myShell> exit
Exiting myShell. Goodbye!
```

---

## Limitations

- Multi-pipe chains (`cmd1 | cmd2 | cmd3`) are not supported
- Redirection and pipes cannot be combined in a single command
- Command history is stored in memory only and lost on exit
