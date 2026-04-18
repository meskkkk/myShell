#include "shell.h"
#include <string.h>

/*
 parse_input: Splits the raw input string into separate arguments.
 It detects special shell operators (<, >, &) and populates the Command struct.
 */
void parse_input(char *input, Command *cmd) {
    //  Initialize the struct values just to be safe
    cmd->arg_count = 0;
    cmd->is_background = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;

    //Tokenize the input string using spaces and tabs as delimiters
    char *token = strtok(input, " \t\n");

    // Loop through all tokens until the end of the string
    while (token != NULL) {
        
        // Detect background execution flag
        if (strcmp(token, "&") == 0) {
            cmd->is_background = 1;
        } 
        // Detect input redirection
        else if (strcmp(token, "<") == 0) {
            
            token = strtok(NULL, " \t\n");
            if (token != NULL) {
                cmd->input_file = token;
            } else {
                printf("Error: Expected filename after '<'\n");
            }
        } 
        // Detect output redirection
        else if (strcmp(token, ">") == 0) {
          
            token = strtok(NULL, " \t\n");
            if (token != NULL) {
                cmd->output_file = token;
            } else {
                printf("Error: Expected filename after '>'\n");
            }
        } 
        // Normal arguments (commands and flags like 'ls' or '-l')
        else {
            cmd->args[cmd->arg_count] = token;
            cmd->arg_count++;
        }

        // Move to the next token
        token = strtok(NULL, " \t\n");
    }

    // NULL-terminate the arguments array. 
    cmd->args[cmd->arg_count] = NULL;
}
