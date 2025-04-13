#include <fcntl.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

const char* shell_name = "MICRO";
#define MAX_SIZE 100000

int main() {
    char buffer[MAX_SIZE];
    while (1) {
        printf("%s > ", shell_name);
        fflush(stdin);

        if (fgets(buffer, MAX_SIZE, stdin) == NULL) {
            break;
        }

        buffer[strlen(buffer) - 1] = '\0';  // Remove newline character
        if (*buffer == '\0') { // Empty command
            continue;
        }

        if (strncmp(buffer, "export ", 7) == 0) {
            char *name = buffer + 7; // Skip "export "
            char *value = strchr(name, '=');
            if (value != NULL) {
                *value = '\0'; // Split name and value
                value++;
                if (setenv(name, value, 1) == -1) { // Set the environment variable
                    printf("setenv\n");
                }
            } else {
                fprintf(stderr, "Invalid export format. Use name=value.\n");
            }
            continue;
        }

        // Handle variable assignment (e.g., x=2)
        if (strchr(buffer, '=') != NULL) {
            char *name = strtok(buffer, "=");
            char *value = strtok(NULL, "=");
            if (name != NULL && value != NULL) {
                if (setenv(name, value, 1) == -1) { // Set the environment variable
                    printf("setenv\n");
                }
            } else {
                fprintf(stderr, "Invalid assignment format. Use name=value.\n");
            }
            continue;
        }

        // Handle commands with embedded environment variables
        char expanded[MAX_SIZE] = ""; // Buffer for expanded command
        char *start = buffer;         // Pointer to traverse the input string
        char *dollar = NULL;          // Pointer to the '$' character

        while ((dollar = strchr(start, '$')) != NULL) {
            // Copy everything before the '$' into the expanded buffer
            strncat(expanded, start, dollar - start);

            // Extract the variable name after the '$'
            char *name = dollar + 1; // Skip the '$'
            char *end = strpbrk(name, " \t\n"); // Find the end of the variable name (space, tab, or newline)
            char var_name[MAX_SIZE];
            if (end != NULL) {
                strncpy(var_name, name, end - name);
                var_name[end - name] = '\0'; 
                start = end; // Move the start pointer after the variable name
            } else {
                strcpy(var_name, name);
                start = dollar + strlen(dollar);
            }

            
            char *value = getenv(var_name);
            if (value != NULL) {
                strcat(expanded, value);
            } else {
                fprintf(stderr, "Environment variable %s not found.\n", var_name);
            }
        }

        // Append the remaining after the last '$'
        strcat(expanded, start);

        // Handle I/O redirection
        char *input_file = NULL;
        char *output_file = NULL;
        char *stderr_file = NULL;
        int append = 0; 

        // Check for input redirection ('<')
        char *input_redirect = strchr(expanded, '<');
        if (input_redirect != NULL) {
            *input_redirect = '\0'; // Split the command
            input_file = strtok(input_redirect + 1, " \t");
        }

        
        char *output_redirect = strchr(expanded, '>');
        if (output_redirect != NULL && *(output_redirect - 1) != '2') { // Ensure it's not '2>'
            if (*(output_redirect + 1) == '>') { // Check for '>>'
                append = 1;
                *output_redirect = '\0';
                output_file = strtok(output_redirect + 2, " \t");
            } else {
                *output_redirect = '\0'; 
                output_file = strtok(output_redirect + 1, " \t");
            }
        }

        
        char *stderr_redirect = strstr(expanded, "2>");
        if (stderr_redirect != NULL) {
            *stderr_redirect = '\0'; 
            stderr_file = strtok(stderr_redirect + 2, " \t");
        }

        // Fork and execute the command
        pid_t pid = fork();
        if (pid == -1) {
            printf("fork\n");
            continue;
        } else if (pid == 0) {
            // Handle input redirection
            if (input_file != NULL) {
                int fd = open(input_file, O_RDONLY);
                if (fd == -1) {
                    printf("open input file");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            // Handle output redirection
            if (output_file != NULL) {
                int fd;
                if (append) {
                    fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                } else {
                    fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                if (fd == -1) {
                    printf("open output file");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // Handle stderr redirection
            if (stderr_file != NULL) {
                int fd = open(stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    printf("open stderr file");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDERR_FILENO);
                close(fd);
            }

            // Tokenize the expanded command and execute it
            char *args[MAX_SIZE];
            char *token = strtok(expanded, " ");
            int i = 0;
            while (token != NULL) {
                args[i] = token;
                token = strtok(NULL, " ");
                i++;
            }
            args[i] = NULL;

            if (execvp(args[0], args) == -1) {
                printf("execvp");
                exit(EXIT_FAILURE);
            }
        }

        // Parent process waits for the child process to finish
        int status;
        if (wait(&status) == -1) {
            printf("wait");
        }
    }
    return 0;
}
