#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 512

void welcome();

void init();

void terminal(char *initialPATH);

void tokenizer(char input[], char *History[]);

void saveHistory(char input[], char *History[]);

void commandHub(char *systemInput[], char *History[]);

void externalCommand(char *command[]);

void cd(char *command[]);

void getpath(char *command[]);

void setpath(char *command[]);

void history(char *History[]);

void lastCommand(char *History[]);

void relativeCommand(char *command[], char *History[]);

void specificCommand(char *command[], char *History[]);

void exitShell(char *PATH, char *History[]);

int main(void)
{
    welcome();
    init();
}

void welcome()
{
    printf("------------------\n");
    printf("   Simple Shell   \n");
    printf("------------------\n\n");
}

void init()
{
    char *initialPATH = getenv("PATH");

    chdir(getenv("HOME"));
    char c[1024];
    printf("HOME: %s\n", getcwd(c, sizeof(c)));
    printf("\n");
    printf("PATH: %s\n\n", initialPATH);

    terminal(initialPATH);
}

void terminal(char *initialPATH)
{

    char *History[21];
    char *emptyCheck = malloc(1);
    strcat(emptyCheck, "\0");
    for (int i = 0; i < 21; i++)
    {
        History[i] = malloc(MAX);
        strcpy(History[i], emptyCheck);
    }

    while (1)
    {

        char input[513] = {'\t'};
        printf("> ");
        input[MAX] = '\n';
        if (fgets(input, 514, stdin) == NULL)
        {
            printf("%s\n", initialPATH);
            exitShell(initialPATH, History);
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
            printf(">");
            if (fgets(input, 514, stdin) == NULL)
            {

                exitShell(initialPATH, History);
                exit(0);
            }
        }

        if (strcmp(input, "exit\n") == 0)
        {
            exitShell(initialPATH, History);
            exit(0);
        }
        if (input[0] != '\n' && input[0] != ' ')
        {
            tokenizer(input, History);
        }
    }
}

void tokenizer(char input[], char *History[])
{

    char *systemInput[50];
    char toSave[MAX];

    strcpy(toSave, input);

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
    }
    systemInput[index] = NULL;

    if (strcmp(systemInput[0], "!!") == 0)
    {
        commandHub(systemInput, History);
    }

    else if (strncmp(systemInput[0], "!-", 2) == 0 ||
             strncmp(systemInput[0], "!", 1) == 0)
    {
        commandHub(systemInput, History);
    }

    else
    {
        saveHistory(toSave, History);

        commandHub(systemInput, History);
    }
}

void saveHistory(char input[], char *History[])
{

    if (strcmp(History[19], "\0"))
    { //if the history is full
        for (int i = 0; i < 20; i++)
        {
            strcpy(History[i], History[i + 1]);
        }

        free(History[20]);
        int i = 0;

        while (input[i] != '\n')
        {
            i++;
        }

        input[i] = '\0';

        char *value = malloc(MAX);
        value = strcat(value, input);

        strcpy(History[19], value);
    }

    else if (!strcmp(History[19], "\0"))
    { // if the history is not full
        int i = 0;
        while (input[i] != '\n')
        {
            i++;
        }
        input[i] = '\0';

        char *value = malloc(MAX);
        value = strcat(value, input);
        int j = 0;
        while (strcmp(History[j], "\0"))
        {
            j++;
        }
        strcpy(History[j], value);
    }
}

void commandHub(char *systemInput[], char *History[])
{

    if (!strcmp(systemInput[0], "cd"))
    {
        cd(systemInput);
    }
    else if (!strcmp(systemInput[0], "getpath"))
    {
        getpath(systemInput);
    }
    else if (!strcmp(systemInput[0], "setpath"))
    {
        setpath(systemInput);
    }
    else if (!strcmp(systemInput[0], "history"))
    {
        history(History);
    }
    else if (!strcmp(systemInput[0], "!!"))
    {
        lastCommand(History);
    }
    else if (!strncmp(systemInput[0], "!-", 2))
    {
        relativeCommand(systemInput, History);
    }
    else if (!strncmp(systemInput[0], "!", 1))
    {
        specificCommand(systemInput, History);
    }
    else
    {
        externalCommand(systemInput);
    }
}

void externalCommand(char *command[])
{
    pid_t c_pid, pid;
    int status;

    c_pid = fork();

    if (c_pid == -1)
    {
        perror("Error: fork failed");
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
            perror("Error: wait failed");
            _exit(1);
        }
    }
}

void cd(char *command[])
{

    if (command[1] != NULL && *command[1] != '~' && command[2] == NULL)
    {
        int i = chdir(command[1]);
        if (i < 0)
        {
            printf("Could not change directory to: %s. Not directory\n", command[1]);
        }
        else
        {
            char cwd[1024];
            printf("%s\n", getcwd(cwd, sizeof(cwd)));
        }
    }
    else if (command[1] != NULL && *command[1] != '~' && command[2] != NULL)
    {
        printf("Directory could not be changed. Too many parameters\n");
    }
    else
    {
        int i = chdir(getenv("HOME"));
        if (i < 0)
        {
            printf("Directory could not be changes\n");
        }
        else
        {
            char cwd[1024];
            printf("%s\n", getcwd(cwd, sizeof(cwd)));
        }
    }
}

void getpath(char *command[])
{

    if (strcmp(command[0], "getpath") == 0 && command[1] == NULL)
    {
        printf("$ Current Path: %s\n", getenv("PATH"));
    }
    else if (strcmp(command[0], "getpath") == 0 && command[1] != NULL)
    {
        printf("Error: No such path\n");
    }
}

void setpath(char *command[])
{
    if (strcmp(command[0], "setpath") == 0 && command[1] != NULL && command[2] != NULL)
    {
        printf("Error: Too many parameters\n");
    }

    else if (strcmp(command[0], "setpath") == 0 && command[1] != NULL)
    {
        setenv("PATH", command[1], 1);
        printf("$ Path Set To: %s\n", getenv("PATH"));
    }
    else if (strcmp(command[0], "setpath") == 0 && command[1] == NULL)
    {
        printf("Error: No path provided\n");
    }
}
void history(char *History[])
{

    if (!strcmp(History[0], "\0"))
    {
        printf("No commands entered yet\n");
    }
    else
    {
        for (int i = 0; i < 20; i++)
        {

            if (strcmp(History[i], "\0"))
            {
                printf("%d : %s \n", (i + 1), History[i]);
            }
        }
    }
}

void lastCommand(char *History[])
{
    int x = 0;
    if (!strcmp(History[0], "\0"))
    {
        printf("Error, no commands in history to execute \n");
    }
    else
    {
        char input[MAX];

        int j = 0;
        while (strcmp(History[j], "\0"))
        {
            j++;
        }
        strcpy(input, History[j - 1]);
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
        }
        systemInput[index] = NULL;

        commandHub(systemInput, History);
    }
}
void specificCommand(char *command[], char *History[])
{

    char *value = strtok(command[0], "!");
    if (value != NULL)
    {
        //ascii digits conversion
        int x = atoi(value);
        if (x < 1 || x > 20)
        {
            printf("Error : index entered out of range \n");
        }
        else
        {
            if (!strcmp(History[x - 1], "\0"))
            {
                printf("Error, no commands in history to execute \n");
            }
            else
            {
                char input[MAX];
                strcpy(input, History[x - 1]);
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
                }
                systemInput[index] = NULL;
                commandHub(systemInput, History);
            }
        }
    }
}

void relativeCommand(char *command[], char *History[])
{
    char *value = strtok(command[0], "!-");
    if (value != NULL)
    {
        int x = atoi(value);
        if (x < 1 || x > 20)
        {
            printf("Error : index entered out of range \n");
        }
        else
        {
            int j = 1;
            while (strcmp(History[j], "\0"))
            {
                j++;
            }
            if (!strcmp(History[j - x], "\0") || j - x < 0)
            {
                printf("Error, no commands in history to execute \n");
            }
            else
            {
                char input[MAX];
                strcpy(input, History[j - x]);
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
                }
                systemInput[index] = NULL;
                commandHub(systemInput, History);
            }
        }
    }
}

void exitShell(char *PATH, char *History[])
{
    for (int i = 0; i < 21; i++)
    {
        free(History[i]);
    }
    printf("\nCurrent path: %s\n", getenv("PATH"));
    setenv("PATH", PATH, 1);
    printf("\n");
    printf("Restored path to: %s\n\n", getenv("PATH"));
    printf("Exiting...\n");
    printf("\n");
    exit(0);
}
