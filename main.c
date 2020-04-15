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

int idx;
int background = 0;
int token_counter(char * line, char * letter);
int command_counter(char * line, char letter);
int custom_fork(char ** arguments);
int custom_execute(char ** arguments);
// void mycd(char ** arguments);
int check_background(char ** arguments);
char ** get_splited_args(char * com, char * letter);
void fatal(char * message, int code);

int main()
{
    while (1)
    {
        int n = 0;
        int k = 0;
        int c;
        int b;
        int command_number;
        int background_number;
        int token_number;
        char * com;
        char * com2;
        char * com3;
        char * ptr;
        char * temp;
        char * t_command;
        char command[MAX_SIZE] = {'\0'};
        char hostname[MAXHOSTNAMELEN] = {'\0'};

        // initialize process number
        idx = 0;

        // get host name
        int h = gethostname(hostname, MAXHOSTNAMELEN);
        if (h) 
        {
            perror("Can not get hostname");
            exit(EXIT_FAILURE);
        }

        // print shell prompt
        printf("%s@%s:%s >>> ", getenv("USER"), hostname, getenv("PWD"));
        
        // get input until press "ENTER" 
        while ((c = fgetc(stdin)))
        {
            // c == "ENTER"
            if (c == 10)
            {
                // if there are no commands, skip
                if (strlen(command) == 0)
                    break;

                command[n] = '\0';

                // TODO: add command in history 

                if ((temp = strdup(command)) == NULL)
                    fatal("Can not allocate memory!", EXIT_FAILURE);
                strcpy(temp, command);

                command_number = token_counter(temp, ";");

                // execute "m" times 
                for (int m = 0; m < command_number; m++)
                {
                    if ((t_command = strdup(command)) == NULL)
                        fatal("Can not allocate memory!", EXIT_FAILURE);
                    strcpy(t_command, command);

                    // store "m" th command in com 
                    com = strtok(t_command, ";");
                    for (int i = 0; i < m; i++)
                        com = strtok(NULL, ";");


                    background_number = command_counter(com, '&');
                    
                    // 백그라운드 개수 만큼 잘라서 arguments로 만듬
                    char ** arguments = get_splited_args(com, "&");
                    
                    // TODO : 뒤에 &가 붙어서 백그라운드로 실행해야 된다면 background = 1하고 
                    // pipe -> redirection 검사 후 실행 해줘야 함!
                    b = 0;
                    
                    for (int i = 0; *(arguments + i) != NULL; i++)
                    {
                        background = 0;
                        if (b < background_number)
                        {
                            background = 1;
                            b++;
                        }

                        char ** arguments2;
                        arguments2 = get_splited_args(*(arguments + i), " ");
                        custom_fork(arguments2);
                    }
                }
                break;
            }
            else
            {
                command[n++] = (char) c;
            }
        }
    }
}

int command_counter(char * line, char letter)
{
    int counter = 0;
    for(int i = 0; i < strlen(line); i++)
    {
        if (*(line + i) == letter)
            counter++;
    }
    return counter;
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
        custom_execute(arguments);
        exit(0);
    }
    // execute parent process
    else
    {
        if (background)
        {
            printf("[%d] %d\n", ++idx, pid);
            waitpid(0, &status, WNOHANG);
        }
        else
        {
            if((pid = waitpid(pid, &status, 0)) < 0)
                fatal("Can not wait!", 1);
        }
    }
    return 0;
}

int custom_execute(char ** arguments)
{
    execvp(* arguments, arguments);
    return 0;
}

void fatal(char * message, int code)
{
    perror(message);
    exit(code);
}

char ** get_splited_args(char * com, char * letter)
{
    int token_number;
    char * t_com;
    char * tt_com;
    char ** arguments;

    if ((t_com = strdup(com)) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    
    if ((tt_com = strdup(com)) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    

    // use when store
    strcpy(t_com, com);

    // use when count
    strcpy(tt_com, com);

    // count the number of tokens in one command
    token_number = token_counter(tt_com, letter);

    if ((arguments = (char **)malloc((token_number + 1) * sizeof(char *))) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);

    // add tokens in arguments
    *arguments = strtok(t_com, letter);
    for (int i = 1; i < token_number; i++)
        *(arguments + i) = strtok(NULL, letter);
    
    // fill NULL pointer in last index
    *(arguments + token_number) = NULL;
    
    return arguments;
}