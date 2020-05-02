#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 512

void welcome();
void init();
void terminal(char *initialPATH);
void externalCommand(char *command[]);
void internalCommand(char *command[]);
void exitShell(char *PATH);

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
    char *initialPATH = getenv("PATH");
    chdir(getenv("HOME"));
    char c[1024];
    printf("Current Directory: %s\n", getcwd(c, sizeof(c)));
    printf("\n");
    printf("Current Path:  %s\n\n", initialPATH);
    terminal(initialPATH);
}
void terminal(char *initialPATH)
{
    while (1)
    {
        char input[513] = {'\t'};
        printf("> ");
        input[MAX] = '\n';
        if (fgets(input, 514, stdin) == NULL)
        {
            printf("\n");
            exitShell(initialPATH);
            exit(0);
        }
        while (input[MAX] != '\n')
        {

            printf("Error: too many characters.\n");
            input[MAX] = '\n';
            char c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            };
            printf("> ");
            if (fgets(input, 514, stdin) == NULL)
            {
                printf("\n");
                exitShell(initialPATH);
                exit(0);
            }
        }

        if (strcmp(input, "exit\n") == 0)
        {
            printf("\n");
            exitShell(initialPATH);
            exit(0);
        }
        if (strcmp(input, "exit\n") == 0)
        {
            exitShell(initialPATH);
            exit(0);
        }

        char *systemInput[50];
        char *inputToken = strtok(input, " '\t' \n | < > & ;");
        int index = 0;
        if (inputToken != NULL)
        {
            while (inputToken != NULL)
            {
                systemInput[index] = inputToken;
                index++;
                inputToken = strtok(NULL, " '\t' \n | < > & ;");
            }
            systemInput[index] = NULL;
            char *builtIn[] = {"cd", "getpath", "setpath"};
            if (!strcmp(systemInput[0], builtIn[0]) || !strcmp(systemInput[0], builtIn[1]))
            {
                internalCommand(systemInput);
            }
            else
            {
                externalCommand(systemInput);
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
void internalCommand(char *command[])
{
    if (strcmp(command[0], "cd") == 0)
    {
        if (command[1] != NULL && *command[1] != '~')
        {
            int i = chdir(command[1]);
            if (i < 0)
            {
                printf("Directory could not be changed\n");
            }
            else
            {
                char cwd[1024];
                printf("%s\n", getcwd(cwd, sizeof(cwd)));
            }
        }
        else
        {
            int i = chdir(getenv("HOME"));
            if (i < 0)
            {
                printf("Directory could not be changed\n");
            }
            else
            {
                char cwd[1024];
                printf("%s\n", getcwd(cwd, sizeof(cwd)));
            }
        }
    }

    if (strcmp(command[0], "getpath") == 0 && command[1] == NULL)
    {
        printf("$ Current Path: %s\n", getenv("PATH"));
    }
    else if (strcmp(command[0], "getpath") == 0 && command[1] != NULL)
    {
        printf("Error: No such path\n");
    }

    if (strcmp(command[0], "setpath") == 0 && command[1] != NULL)
    {
        setenv("PATH", command[1], 1);
        printf("$ Path Set To: %s\n", getenv("PATH"));
    }
    else if (strcmp(command[0], "setpath") == 0 && command[1] == NULL)
    {
        printf("Error: No path provided\n");
    }
}

void exitShell(char *PATH)
{
    printf("\nCurrent path: %s\n", getenv("PATH"));
    setenv("PATH", PATH, 1);
    printf("\n");
    printf("Restored path to: %s\n\n", getenv("PATH"));
    printf("Exiting...\n");
    printf("\n");
    exit(0);
}