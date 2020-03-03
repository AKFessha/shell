#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 512

void welcome();
void terminal();

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

    }
}