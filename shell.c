#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 512

void welcome();
void terminal();
void externalCommand(char *command[]);

int main(void)
{
    welcome();
    terminal();
}

void welcome()
{
    printf("-------------------------\n");
    printf("          MY SHELL       \n");
    printf("-------------------------\n");
}
void terminal()
{
    while (1)
    {
        char input[513] = {'\t'};
        printf("> ");
        input[MAX] = '\n';
        if (fgets(input, 514, stdin) == NULL)
        {
            printf("\n");
            exit(0);
        }
        while (input[MAX] != '\n')
        {

            printf("Error: too many characters.\n");
            input[MAX] = '\n';
            printf("> ");
            if (fgets(input, 514, stdin) == NULL)
            {
                printf("\n");
                exit(0);
            }
        }

        if (strcmp(input, "exit\n") == 0)
        {
            printf("\n");
            exit(0);
        }

        char *inputToken = strtok(input, " '\t' \n | < > & ;");
        if (inputToken != NULL)
        {
            while (inputToken != NULL)
            {
                printf("\"%s\"\n", inputToken);
                inputToken = strtok(NULL, " '\t' \n | < > & ;");
            }
        }
    }
}

void externalCommand(char *command[])
{
    pid_t c_pid, pid;
    int status;

    c_pid = fork();

    if (c_pid == -1)
    {
        printf("fork failed");
        _exit(1);
    }
    if (c_pid == 0)
    {
        execvp(command[0], command);
        perror(command[0]);
        _exit(1);
    }
    else if (c_pid > 0)
    {

        if (pid = wait(&status) < 0)
        {
            perror("error: wait failed");
            _exit(1);
        }
    }
}