#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/param.h>

#define MAX_SIZE     1000
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

int token_counter(char * line, char * letter);
int custom_fork(char ** arguments);
int custom_execute(char ** arguments);
void my_cd(char ** arguments);

int main()
{
    while (1)
    {
        int n = 0;
        int m = 0;
        int c;
        int command_number;
        int token_number;
        char * com;
        char * t_command;
        char * t_com;
        char * tt_com;
        char ** arguments;
        char command[MAX_SIZE] = {'\0'};
        char hostname[MAXHOSTNAMELEN] = {'\0'};

        // get host name
        int h = gethostname(hostname, MAXHOSTNAMELEN);
        if (h) 
        {
            perror("Can not get hostname");
            exit(EXIT_FAILURE);
        }

        // print shell prompt
        printf("%s@%s:%s >>> ", (char *)getenv("USER"), hostname, (char *)getenv("PWD"));
        
        // get input until press "ENTER" 
        while ((c = fgetc(stdin)))
        {
            if (c == 10)
            {
                // if there are no commands, skip
                if (strlen(command) == 0)
                    break;

                command[n] = '\0';

                if ((t_command = (char *)malloc(strlen(command) + 1)) == NULL)
                {
                    perror("Can not allocate memory!");
                    exit(EXIT_FAILURE);
                }

                // TODO: add command in history 

                strcpy(t_command, command);
                
                // count the number of the commands
                command_number = token_counter(t_command, ";");

                while (m < command_number)
                {
                    strcpy(t_command, command);

                    // get "m"th command
                    com = strtok(t_command, ";");
                    for (int i = 0; i < m; i++)
                        com = strtok(NULL, ";");

                    if ((t_com = (char *)malloc(strlen(com) + 1)) == NULL)
                    {
                        perror("Can not allocate memory!");
                        exit(EXIT_FAILURE);
                    }
                    if ((tt_com = (char *)malloc(strlen(com) + 1)) == NULL)
                    {
                        perror("Can not allocate memory!");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(t_com, com);
                    strcpy(tt_com, com);

                    // count the number of tokens in one command
                    token_number = token_counter(tt_com, " ");

                    if ((arguments = (char **)malloc((token_number + 1) * sizeof(char *))) == NULL)
                    {
                        perror("Can not allocat memory!");
                        exit(EXIT_FAILURE);
                    }

                    // add tokens in arguments
                    *arguments = strtok(t_com, " ");
                    for (int i = 1; i < token_number; i++)
                        *(arguments + i) = strtok(NULL, " ");
                    // fill NULL pointer in last index for execvp
                    *(arguments + token_number) = NULL;

                    // TODO: check is pipe ?

                    // TODO: check is redirection ?

                    // execute the command
                    int v;
                    if ((v = custom_fork(arguments)) == 1)
                        break;
                    

                    // check arguments
                    // for (int i = 0; i < token_number; i++)
                    //     printf("%s\n", *(arguments + i));
                    
                    m++;
                }
                free(t_command);
                break;
            }
            else
            {
                command[n++] = (char) c;
            }
        }
    }
}

int token_counter(char * line, char * letter)
{
    int counter = 1;
    char * tok;

    if ((tok = strtok(line, letter)) != NULL)
    {
        while (1)
        {
            if ((tok = strtok(NULL, letter)) == NULL)
                break;
            else
                counter++;
        }
    }
    return counter;
}

int custom_fork(char ** arguments)
{
    int status;
    int pid;
    if ((pid = fork()) < 0)
    {
        perror("Can not fork!");
        return 1;
    }
    // execute child process
    else if (pid == 0)
    {
        int v;
        custom_execute(arguments);
        exit(0);
    }
    // execute parent process
    else
    {
        if((pid = waitpid(pid, &status, 0)) < 0)
        {
            perror("Can not wait!");
            return 1;
        }
    }
    
    return 0;
}

int custom_execute(char ** arguments)
{
    // TODO: make custom cd command
    // if (strcmp(arguments[0], "cd") == 0)
    //     my_cd(arguments);
    execvp(* arguments, arguments);
    return 0;
}

void my_cd(char ** arguments)
{
    // TODO: make custom cd command
}