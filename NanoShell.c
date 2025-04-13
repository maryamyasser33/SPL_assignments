#include <fcntl.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

const char* shell_name = "Nano >";
#define MAX_SIZE 100000

int main() {
    char buffer[MAX_SIZE];
    while (1) {
        printf("%s >", shell_name);
        fflush(stdin);

        if (fgets(buffer, MAX_SIZE, stdin) == NULL) {
            break;
        }

        buffer[strlen(buffer) - 1] = 0;
        if (*buffer == 0) { // Empty command
            continue;
        }

        // Handle "export" command
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
                var_name[end - name] = '\0'; // Null-terminate the variable name
                start = end; // Move the start pointer after the variable name
            } else {
                strcpy(var_name, name); // If no space, take the rest of the string
                start = dollar + strlen(dollar); // Move start to the end of the string
            }

            // Get the value of the environment variable
            char *value = getenv(var_name);
            if (value != NULL) {
                strcat(expanded, value); // Append the value to the expanded buffer
            } else {
                fprintf(stderr, "Environment variable %s not found.\n", var_name);
            }
        }

        // Append the remaining part of the string after the last '$'
        strcat(expanded, start);

        // Fork and execute the command
        pid_t pid = fork();
        if (pid == -1) {
            printf("fork\n");
            continue;
        } else if (pid == 0) {
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
                printf("execvp\n");
                exit(EXIT_FAILURE);
            }
        }

        // Parent process waits for the child process to finish
        int status;
        if (wait(&status) == -1) {
            printf("wait\n");
        }
    }
    return 0;
}
