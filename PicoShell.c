#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 1024

void free_args(char **args)
{
    if (strcmp(args[0], "exit") == 0) {
	free_args(args);
	exit(0);

    }
}

void execute_command(char **argv)
{
    if (strcmp(argv[0], "exit") == 0) {
	free_args(argv);
	exit(0);
    } else if (strcmp(argv[0], "pwd") == 0) {
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd))) {
	    printf("%s\n", cwd);
	} else {
	    printf("Error in pwd\n");
	    return;
	}
    } else if (strcmp(argv[0], "cd") == 0) {
	char cwd[1024];
	if (argv[1] == NULL) {
	    printf("Missing argument in cd\n");
	} else if (chdir(argv[1]) != 0) {
	    printf("Error in cd\n");
	    return;
	}
    } else if (strcmp(argv[0], "echo") == 0) {
	{
	    for (int i = 0; argv[i]; i++)
		printf("%s ", argv[i]);
	}
	printf("\n");
    } else {
	// fork a child process to execute the command
	pid_t pid = fork();
	if (pid < 0) {
	    printf("Error in fork\n");
	} else if (pid == 0) {
	    // child
	    execvp(argv[0], argv);
	    exit(EXIT_FAILURE);
	} else {
	    wait(NULL);		// mot interested in the return value of exit from child process
	}
    }
}

char **parse_input(char *input)
{
    char **args = malloc(sizeof(char *) * MAX_SIZE);
    if (!args) {
	printf("Error in Dynamic Allocation\n");
	exit(EXIT_FAILURE);
    }

    char *token = strtok(input, " ");
    int index = 0;
    while (token) {

	args[index] = strdup(token);

	if (!args[index]) {
	    printf("strdup error");
	    exit(EXIT_FAILURE);
	}

	index++;
	token = strtok(NULL, " ");
    }

    args[index] = NULL;

    return args;
}


int main()
{
    char input[MAX_SIZE];
    char **argv;
    while (1) {
	printf("PICOSHELL > ");
	if (!fgets(input, MAX_SIZE, stdin)) {
	    printf("Error Taking input!\n");
	    return -1;
	}
	input[strlen(input) - 1] = 0;	// null char
	argv = parse_input(input);
	if (argv[0]) {
	    execute_command(argv);
	}
	for (int i = 0; argv[i] != NULL; i++) {
	    free(argv[i]);
	}
	free(argv);
    }
    return 0;
}
