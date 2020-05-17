//Link Section (Import statements)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//Defining constant for mamximum command length , it is at most 512 characters long
#define MAX 512

//Function Declarations
void welcome();

void init();

void terminal(char *initialPATH, char *initialDIR);

void load_commandHistory(char *initialDIR, char *History[]);

void load_Aliases(char *initialDIR, char *aliasArray[11][2]);

void tokenizer(char *aliasArray[11][2], char input[], char *History[]);

void saveHistory(char input[], char *History[]);

void commandHub(char *aliasArray[11][2], char *systemInput[], char *History[]);

void cd(char *command[]);

void getpath(char *command[]);

void setpath(char *command[]);

void history(char *History[]);

void alias(char *aliasArray[11][2], char *command[]);

void unalias(char *aliasArray[11][2], char *command[]);

void lastCommand(char *aliasArray[11][2], char *History[]);

void relativeCommand(char *aliasArray[11][2], char *command[], char *History[]);

void specificCommand(char *aliasArray[11][2], char *command[], char *History[]);

void externalCommand(char *command[]);

void Check_Circular(char *aliasArray[11][2], char *command[]);

void changeToAliasedCommand(char *aliasArray[11][2], char *systemInput[], char *History[]);

int checkIfInAlias(char *aliasArray[11][2], char *systemInput);

void save_command(char *History[], char *initialDIR);

void save_alias(char *aliasArray[11][2], char *initialDIR);

void exitShell(char *aliasArray[11][2], char *initialPATH, char *initialDIR, char *History[]);

//Global Variables used to detect an infinite loop/command
int infinitLoopDetected = 0;
char infinitCommand[MAX] = {'\0'};

//main function , prints welcome message and calls initialiser
int main(void)
{
    welcome();
    init();
}

//prints welcome message
void welcome()
{
    printf("------------------\n");
    printf("   Simple Shell   \n");
    printf("------------------\n\n");
}

//Function to initialise values and calls terminal function to start simple shell
void init()
{
    char d[1024];
    char *initialDIR = getcwd(d, sizeof(d));

    printf("Current Directory: %s\n\n", initialDIR);

    char *initialPATH = getenv("PATH");

    chdir(getenv("HOME"));
    char c[1024];
    printf("Home Directory: %s\n", getcwd(c, sizeof(c)));
    printf("\n");
    printf("Path: %s\n\n", initialPATH);

    terminal(initialPATH, initialDIR);
}

//Function for terminal , initialises values and takes in input
void terminal(char *initialPATH, char *initialDIR)
{

    //creates the history and alias arrays
    char *History[21];
    char *aliasArray[11][2];

    //used to determine an empty value
    char *emptyCheck = malloc(1);

    strcat(emptyCheck, "\0");

    //malloc for each member of history array , and initialiases the values as being empty
    for (int i = 0; i < 21; i++)
    {
        History[i] = malloc(MAX);
        strcpy(History[i], emptyCheck);
    }

    // do the same process for the 2d alias array
    for (int i = 0; i < 11; i++)
    {
        aliasArray[i][0] = malloc(MAX);
        aliasArray[i][1] = malloc(MAX);

        strcpy(aliasArray[i][0], emptyCheck);
        strcpy(aliasArray[i][1], emptyCheck);
    }

    //loads up any existing command history and aliases
    load_commandHistory(initialDIR, History);
    load_Aliases(initialDIR, aliasArray);

    //infinite loop that continues to take input until it reads in exit command (in which it breaks from loop and exits)
    while (1)
    {

        char input[513] = {'\t'};
        //Display prompt
        printf("> ");
        input[MAX - 1] = '\n';

        if (fgets(input, 514, stdin) == NULL)
        {

            printf("\n");
            exitShell(aliasArray, initialPATH, initialDIR, History);
            exit(0);
        }

        //If input buffer contains too many characters
        while (input[MAX - 1] != '\n')
        {
            printf("Error: too many characters.\n");
            input[MAX - 1] = '\n';
            char c;
            printf("Press enter to continue...");
            while ((c = getchar()) != '\n' && c != EOF)
            {
            };
            printf("> ");
            if (fgets(input, 514, stdin) == NULL)
            {
                printf("\n");
                exitShell(aliasArray, initialPATH, initialDIR, History);
                exit(0);
            }
        }
        //if exit command invoked
        if (strcmp(input, "exit\n") == 0)
        {
            printf("\n");
            exitShell(aliasArray, initialPATH, initialDIR, History);
            exit(0);
        }
        //tokenizes the input if it is not empty
        if (input[0] != '\n' && input[0] != ' ')
        {

            tokenizer(aliasArray, input, History);
        }
    }
}

//function to load the command history from file
void load_commandHistory(char *initialDIR, char *History[])
{

    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".hist_list", "r");

    //if file does not exist , nothing happens
    if (fp == NULL)
    {
        printf("...History file not found...\n");
    }
    else
    {

        char buffer[MAX];
        int count = 0;

        while (fgets(buffer, MAX, fp) != NULL)
        {
            int length = strlen(buffer);

            buffer[length] = '\0';
            char val[MAX];

            strtok(buffer, "\n");
            strcpy(val, buffer);
            strcpy(History[count], val);
            count++;
        }
        fclose(fp);
    }
    chdir(getenv("HOME"));
}

// function used to aliases from file
void load_Aliases(char *initialDIR, char *aliasArray[11][2])
{

    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".aliases", "r");

    //if file does not exist , nothing happens
    if (fp == NULL)
    {
        printf("...Alias file not found...\n");
    }
    else
    {

        char buffer[MAX];
        int count = 0;

        while (fgets(buffer, MAX, fp) != NULL)
        {
            int length = strlen(buffer);

            buffer[length] = '\0';
            char name[length];
            char command[length];

            char *input;

            input = strtok(buffer, " ");
            strcpy(name, input);
            strcpy(aliasArray[count][0], name);

            input = strtok(NULL, "\n");
            strcpy(command, input);
            strcpy(aliasArray[count][1], command);
            count++;
        }
        fclose(fp);
    }

    chdir(getenv("HOME"));
}
//function that breaks the input into tokens based on whitespace and certain symbols
//and sends input to correct method.
void tokenizer(char *aliasArray[11][2], char input[], char *History[])
{
    char *systemInput[50];
    char toSave[MAX];

    strcpy(toSave, input);

    char *inputToken = strtok(input, " \t \n | < > & ;");

    int index = 0;
    if (inputToken != NULL)
    {
        while (inputToken != NULL)
        {

            systemInput[index] = inputToken;
            index++;
            inputToken = strtok(NULL, " \t  \n | < > & ;");
        }
    }
    systemInput[index] = NULL;

    if (systemInput[0] == NULL)
    {
        return;
    }

    if (checkIfInAlias(aliasArray, systemInput[0]) != 0)
    {
        saveHistory(toSave, History);
        changeToAliasedCommand(aliasArray, systemInput, History);
    }

    else if (strcmp(systemInput[0], "!!") == 0)
    {

        commandHub(aliasArray, systemInput, History);
    }

    else if (strncmp(systemInput[0], "!-", 2) == 0 || strncmp(systemInput[0], "!", 1) == 0)
    {

        commandHub(aliasArray, systemInput, History);
    }

    else
    {
        saveHistory(toSave, History);
        changeToAliasedCommand(aliasArray, systemInput, History);
    }
}
//Function to save the input to the command history
void saveHistory(char input[], char *History[])
{
    if (strcmp(History[19], "\0"))
    { //if the history is full
        for (int i = 0; i < 19; i++)
        {
            strcpy(History[i], History[i + 1]);
        }
        free(History[19]);
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

//Function that handles input commands , by calling the relevant function to execute command or prints an error if input is invalid
// like a hub, hence the name "commandHub"
void commandHub(char *aliasArray[11][2], char *systemInput[], char *History[])
{

    if (!strcmp(systemInput[0], "unalias"))
    {
        unalias(aliasArray, systemInput);
        return;
    }

    else if (!strcmp(systemInput[0], "cd"))
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
        if (systemInput[1] == NULL)
        {
            history(History);
        }
        else
        {
            printf("Error: History takes no parameter\n");
        }
    }

    else if (!strcmp(systemInput[0], "alias"))
    {
        alias(aliasArray, systemInput);
    }

    else if (!strcmp(systemInput[0], "!!"))
    {
        if (systemInput[1] == NULL)
        {
            lastCommand(aliasArray, History);
        }
        else
        {
            printf("Error: !! takes no parameter\n");
        }
    }

    else if (!strncmp(systemInput[0], "!-", 2))
    {

        if (systemInput[1] == NULL)
        {
            relativeCommand(aliasArray, systemInput, History);
        }
        else
        {
            printf("Error: !-<number> takes additional parameter\n");
            printf("Error: than a number between 1 and 20\n");
        }
    }

    else if (!strncmp(systemInput[0], "!", 1))
    {
        if (systemInput[1] == NULL)
        {
            specificCommand(aliasArray, systemInput, History);
        }
        else
        {
            printf("Error: !<number> takes additional parameter\n");
            printf("Error: than a number between 1 and 20\n");
        }
    }

    else
    {
        externalCommand(systemInput);
    }
}

//function to execute current directory internal command , takes in any parameters specified by user
void cd(char *command[])
{
    if (command[1] != NULL && *command[1] != '~' && command[2] == NULL)
    {
        int i = chdir(command[1]);
        if (i < 0)
        {
            perror(command[1]);
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

//Function to execute getpath internal command , if any arguments are passed in along with getpath command, an error message is printed
void getpath(char *command[])
{

    if (strcmp(command[0], "getpath") == 0 && command[1] == NULL)
    {
        printf("$ Current Path: %s\n", getenv("PATH"));
    }
    else if (strcmp(command[0], "getpath") == 0 && command[1] != NULL)
    {
        printf("Error: getpath takes no paramters\n");
    }
}

//Function to execute setpath internal command , takes in any arguments specified for command
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

//function that executes the history internal command  , prints up to the last 20 previous commands , or an error message if there are no commands in history
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

//Function to execute alias internal command , if successful then it will add the created alias to the alias array
void alias(char *aliasArray[11][2], char *command[])
{
    if (command[1] == NULL)
    {
        int j = 0;
        for (int i = 0; i < 10; i++)
        {
            if (strcmp(aliasArray[i][0], "\0") && strcmp(aliasArray[i][1], "\0"))
            {
                printf("Alias: \"%s\" Command: \"%s\"\n", aliasArray[i][0], aliasArray[i][1]);
                j++;
            }
        }
        if (j == 0)
        {
            printf("No aliases set\n");
        }
        return;
    }

    else if (command[2] == NULL)
    {
        printf("No command set\n");
        return;
    }

    if (!strcmp(command[1], "alias"))
    {
        printf("Error: cannot use alias 'alias'");
        printf("\n");
        return;
    }

    if (!strcmp(command[1], "unalias"))
    {
        printf("Error: cannot use alias 'unalias'");
        printf("\n");
        return;
    }

    char *temp = malloc(MAX);
    strcat(temp, command[2]);

    int i = 3;
    while (command[i] != NULL)
    {
        strcat(temp, " ");
        strcat(temp, command[i]);
        i++;
    }

    char *aliasName = malloc(MAX);
    char *aliasCommand = malloc(MAX);

    strcat(aliasName, command[1]);
    strcat(aliasCommand, temp);

    for (int j = 0; j < 10; j++)
    {
        if (!strcmp(aliasArray[j][0], "\0"))
        {
            strcpy(aliasArray[j][0], aliasName);
            strcpy(aliasArray[j][1], aliasCommand);
            printf("Created alias \"%s\" for the command: \"%s\"\n", command[1], aliasCommand);
            Check_Circular(aliasArray, command);
            return;
        }
        else if (!strcmp(aliasArray[j][0], aliasName))
        {
            printf("Changed alias \"%s\" from the command: \"%s\"\n", command[1], aliasArray[j][1]);
            strcpy(aliasArray[j][1], aliasCommand);
            printf("---to the command: %s\n", aliasCommand);
            return;
        }
    }
    printf("Alias list is full\n");
}

//Function to execute unalias internal command , if successful , it will remove the alias from alias array
void unalias(char *aliasArray[11][2], char *command[])
{
    if (command[1] == NULL)
    {
        printf("No alias provided\n");
    }
    else
    {
        int j = 0;
        for (int i = 0; i < 10; i++)
        {
            if (!strcmp(command[1], aliasArray[i][0]))
            {
                printf("Unliased command: \"%s\" from alias \"%s\"\n", aliasArray[i][1], command[1]);
                strcpy(aliasArray[i][0], "\0");
                strcpy(aliasArray[i][1], "\0");
                j = 1;
            }
        }
        if (j == 0)
        {
            printf("Alias %s does not exist.\n", command[1]);
        }
    }
}

//Function to execute the !! internal history command
void lastCommand(char *aliasArray[11][2], char *History[])
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
        changeToAliasedCommand(aliasArray, systemInput, History);
    }
}

//Function to execute the !<no.> internal history command
void specificCommand(char *aliasArray[11][2], char *command[], char *History[])
{

    char *value = strtok(command[0], "!");
    if (value != NULL)
    {
        int x = atoi(value);
        if (x == 0)
        {
            printf("Error: value entered must be an integer value between 1 and 20\n");
        }
        else if (x < 1 || x > 20)
        {
            printf("Error: index entered out of range\n");
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
                changeToAliasedCommand(aliasArray, systemInput, History);
            }
        }
    }
}

//Function to execute the !-<no.> internal history command
void relativeCommand(char *aliasArray[11][2], char *command[], char *History[])
{

    char *value = strtok(command[0], "!-");
    if (value != NULL)
    {
        int x = atoi(value);
        if (x == 0)
        {
            printf("Error: value entered must be an integer value between 1 and 20\n");
        }
        else if (x < 1 || x > 20)
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
                changeToAliasedCommand(aliasArray, systemInput, History);
            }
        }
    }
}

//function to call and execute an external command via execvp
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

//function used to determine if the created alias causes a circular dependency , if it does then the alias created is unaliased
void Check_Circular(char *aliasArray[11][2], char *command[])
{

    for (int i = 0; i < 10 && (strcmp(aliasArray[i][0], "\0") != 0); i++)
    {
        char *To_check = malloc(sizeof(char));
        strcpy(To_check, aliasArray[i][0]);

        char *Chain_Value = malloc(sizeof(char));
        strcpy(Chain_Value, aliasArray[i][1]);

        int end_of_chain = 0;

        while (!end_of_chain)
        {
            for (int j = 0; j < 10 && (strcmp(aliasArray[j][0], "\0") != 0); j++)
            {

                if (!strcmp(aliasArray[j][0], Chain_Value))
                {
                    if (!strcmp(aliasArray[j][1], To_check))
                    {

                        printf("Circular dependency detected for %s\n Cancelling alias...\n", To_check);
                        unalias(aliasArray, command);
                        free(To_check);
                        free(Chain_Value);
                        return;
                    }
                    strcpy(Chain_Value, aliasArray[j][1]);
                }
            }

            end_of_chain = 1;
        }
        free(To_check);
        free(Chain_Value);
    }
}

// this method will change the input command to contain the aliased command
// if they contain aliases. it will then send the command to the command hub
// it also detects if the program has ran into
// a possible infinite loop. it will loop for 500 if it is an infinite loop and come ou
// of that loop if it reaches 500. it will also inform the user of that

void changeToAliasedCommand(char *aliasArray[11][2], char *systemInput[], char *History[])
{

    if (!strcmp(systemInput[0], "unalias"))
    {
        unalias(aliasArray, systemInput);
        return;
    }

    else if (!strcmp(systemInput[0], "alias"))
    {
        alias(aliasArray, systemInput);
        return;
    }

    int contained = 0;
    int initialCheck = 0;
    while (systemInput[initialCheck] != NULL)
    {

        for (int i = 0; i < 10; i++)
        {
            if (!strcmp(systemInput[initialCheck], aliasArray[i][0]))
            {
                contained = 1;
                break;
            }
        }
        initialCheck++;
    }

    if (contained == 1)
    {
        char *holder[50];

        char *systemInputDuplicate[51];
        int totalSizeOfSystemInput = 0;
        for (int i = 0; systemInput[i] != NULL; i++)
        {
            systemInputDuplicate[i] = systemInput[i];
            totalSizeOfSystemInput++;
        }
        systemInputDuplicate[totalSizeOfSystemInput] = NULL;
        int index = 0;

        while (systemInputDuplicate[index] != NULL)
        {
            int aliasIndex = -1;
            for (int j = 0; j < 10; j++)
            {   //redoing first pass incase alias list is jumbled
                //up
                for (int i = 0; i < 10; i++)
                { //first pass
                    if (strcmp(aliasArray[i][0], "\0"))
                    {
                        if (!strcmp(systemInputDuplicate[index], aliasArray[i][0]))
                        {
                            systemInputDuplicate[index] = aliasArray[i][1];
                            aliasIndex = i;
                        }
                    }
                }
            }
            if (aliasIndex != -1)
            {
                holder[index] = aliasArray[aliasIndex][1];
            }
            else
            {
                holder[index] = systemInput[index];
            }
            index++;
        }

        char holderArray[MAX];
        strcpy(holderArray, holder[0]);
        for (int i = 1; i < index; i++)
        {
            strcat(holderArray, " ");
            strcat(holderArray, holder[i]);
        }
        char *newCommandToken = strtok(holderArray, " ");
        char *newCommand[50];
        index = 0;
        while (newCommandToken != NULL)
        {
            newCommand[index] = newCommandToken;
            index++;
            newCommandToken = strtok(NULL, " ");
        }
        newCommand[index] = NULL;

        if (!strcmp(infinitCommand, holderArray))
        {
            infinitLoopDetected++;
        }

        else
        {
            strcpy(infinitCommand, holderArray);
            infinitLoopDetected = 0;
        }

        if (infinitLoopDetected == 500)
        {
            printf("The shell may be stuck on an infinite loop...\n");
            printf("500 iterations have already been done\n");
            printf("Exiting loop...\n");
            infinitLoopDetected = 0;
        }
        else
        {
            commandHub(aliasArray, newCommand, History);
            //if newCommand is set to redo the last command, an infinite loop will happen
            // this is because the last command is the command which called the alias
            // for example, !-1 should instead be !-2, since calling !-1 will recall this command.
        }
    }
    else
    {

        commandHub(aliasArray, systemInput, History);
    }
    return;
}

//Function used to determine if the input is an alias in alias array
int checkIfInAlias(char *aliasArray[11][2], char *systemInput)
{

    int found = 0;
    for (int i = 0; i < 10; i++)
    {
        if (!strcmp(systemInput, aliasArray[i][0]))
        {
            found = 1;
            break;
        }
    }
    return found;
}

//function to save the command history to file
void save_command(char *History[], char *initialDIR)
{
    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".hist_list", "w+");
    if (fp == NULL)
    {
        printf("Save file not found");
    }
    else
    {
        for (int i = 0; i < 20; i++)
        {
            if (strcmp(History[i], "\0") != 0)
            {
                fprintf(fp, "%s", History[i]);
                if (strcmp(History[i + 1], "\0") != 0)
                {
                    fputs("\n", fp);
                }
            }
        }
        fclose(fp);
    }
}

//function to save the alias array to file
void save_alias(char *aliasArray[11][2], char *initialDIR)
{
    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".aliases", "w+");
    if (fp == NULL)
    {
        printf("Save file not found");
    }
    else
    {
        for (int i = 0; i < 11; i++)
        {
            if (strcmp(aliasArray[i][0], "\0") != 0)
            {
                fprintf(fp, "%s %s", aliasArray[i][0], aliasArray[i][1]);
                if (strcmp(aliasArray[i + 1][0], "\0") != 0)
                {
                    fputs("\n", fp);
                }
            }
        }
        fclose(fp);
    }
}

//function used to properly exit shell  ,printing the exit messages ,  freeing up malloc and saving the aliases and command history
void exitShell(char *aliasArray[11][2], char *initialPATH, char *initialDIR, char *History[])
{
    printf("\nCurrent path: %s\n", getenv("PATH"));

    setenv("PATH", initialPATH, 1);

    printf("\n");
    printf("Restored path to: %s\n\n", getenv("PATH"));
    printf("Saving History..\n");

    save_command(History, initialDIR);
    save_alias(aliasArray, initialDIR);

    for (int i = 0; i < 21; i++)
    {
        free(History[i]);
    }

    for (int i = 0; i < 11; i++)
    {
        free(aliasArray[i][0]);
        free(aliasArray[i][1]);
    }

    printf("Exiting...\n");
    printf("\n");
    exit(0);
}
