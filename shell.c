#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 512

void welcome();
void init();
void terminal(char *initalPATH);
void externalCommand(char *command[]);
void exitShell(char *initalPATH);

int main(void)
{
    welcome();
    init();
}

void welcome()
{
    printf("-------------------------\n");
    printf("          MY SHELL       \n");
    printf("-------------------------\n");
}

void init()
{
    char *initalPATH = getenv("PATH");
    chdir(getenv("HOME"));
    char c[1024];
    printf("Current Directory: %s\n", getcwd(c, sizeof(c)));
    printf("\n");
    printf("Current Path:  %s\n\n", initialPATH);
    terminal(initialPATH);
}
void terminal(char *initalPATH)
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
            char c;
            while ((c = getchar()) == '\n' && c != EOF)
            {
            };
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

        if ((pid = wait(&status)) < 0)
        {
            perror("error: wait failed");
            _exit(1);
        }
    }
}

void exitShell(char *initalPATH)
{
    printf("\nCurrent path: %s\n", getenv("PATH"));
    setenv("PATH", PATH, 1);
    printf("\n");
    printf("Restored path to: %s\n\n", getenv("PATH"));
    printf("Exiting...\n");
    printf("\n");
    exit(0);
}