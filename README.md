# myShell

A lightweight command-line shell built in C, developed as a final project for the
Operating Systems course at the Faculty of Computers and Data Science, Alexandria University.

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

## How to Compile

```bash
make
```

## How to Run

```bash
./myShell
```

---

## Features

- [x] Interactive prompt (`myShell>`)
- [x] Command parsing with arguments
- [x] Built-in commands: `cd`, `pwd`, `exit`, `history`
- [x] Foreground execution
- [x] Background execution with `&` (prints PID)
- [x] Input redirection `<`
- [x] Output redirection `>`
- [x] Pipes `|`
- [x] Signal handling — `Ctrl+C` kills child process only, shell stays alive
- [x] Error handling — command not found, bad paths, fork/pipe failures

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

The entry point of the shell. Displays the `myShell>` prompt, reads user input in a continuous loop, and coordinates calls to the parser and execution modules. Also handles final integration of all components.

---

### `parser.c` — Command Parsing & Tokenization 

The first stage of command processing. Takes the raw input string typed by the user and breaks it down into structured data the rest of the shell can work with.

**Responsibilities:**
- **Tokenization** — Splits the input line into tokens using delimiters (spaces, tabs)
- **Argument building** — Fills the `args[]` array passed to `execvp()`
- **Operator detection** — Scans for and flags the presence of:
  - `&` — background execution
  - `>` — output redirection
  - `<` — input redirection
  - `|` — pipe between two commands
- **Filename extraction** — When `>` or `<` is detected, captures the target/source filename
- **Edge case handling** — Gracefully handles empty input, extra spaces, and newline characters without crashing the shell

The parser fills a shared `Command` struct (defined in `shell.h`) that all other modules read from. `main.c` calls the parser first on every input, then passes the result to `process.c`, `builtin.c`, and `io_signals.c` depending on what was detected.

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
```

---

## Notes

- The shell does not support multi-pipe chains (`cmd1 | cmd2 | cmd3`)
- Redirection and pipes cannot be combined in a single command
- History is stored in memory only and is lost when the shell exits

---

##process.c — Process Execution & Background Jobs

The module responsible for executing external commands by creating and managing processes. It receives structured input from the parser and interacts directly with the operating system using system calls.

**Responsibilities:**
	•	Process creation — Uses fork() to create a child process for each command, ensuring the shell continues running independently
	•	Command execution — Uses execvp() to replace the child process with the requested program using args[]
	•	Execution modes:
	◦	Foreground — The parent process waits for the child using waitpid()
	◦	Background (&) — The shell does not wait and immediately prints the process ID (PID)
	•	Process coordination — Detects if redirection is required and delegates execution to the redirection handler
	•	Error handling — Handles failures in fork() and execvp(), and prints clear error messages if a command is invalid

#Usage Examples

# Basic execution
myShell> ls
main.c  parser.c  process.c  shell.h

# Foreground execution (shell waits)
myShell> sleep 3

# Background execution
myShell> sleep 5 &
[3928]
myShell>

# Invalid command
myShell> abc
myShell: abc: command not found

Notes

	•	Each external command runs in a separate child process
	•	The parent process only waits in foreground mode
	•	Background processes allow the shell to remain responsive
	•	Works seamlessly with parser output and integrates with redirection and pipe modules
---
### builtin.c - Built-in Commands 

- **cd** - Changes directory. Works with `~`, `..`, absolute paths, and relative paths. Shows error if directory doesn't exist.
- **pwd** - Prints the current working directory.
- **exit** - Exits the shell with a goodbye message.
- **history** - Shows all commands typed this session with line numbers.

**History features:**
- Stores up to 100 commands
- Won't store the same command twice in a row
- Old commands get removed automatically when full

**Example:**
```bash
myShell> pwd
/home/user/project

myShell> cd /tmp
myShell> pwd
/tmp

myShell> cd ~
myShell> pwd
/home/user

myShell> ls
myShell> history

Command History:
----------------
   1  pwd
   2  cd /tmp
   3  pwd
   4  cd ~
   5  pwd
   6  ls
   7  history

myShell> exit
Exiting myShell. Goodbye!
```
---

