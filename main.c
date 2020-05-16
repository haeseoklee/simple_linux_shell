//  main.c
//  myshell
//
//  Created by Haeseok Lee on 2020/05/12.
//  Copyright © 2020 Haeseok Lee. All rights reserved.
//
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

int idx, s_stdin, s_stdout;
int fd[2];
int isbackground = 0, ispipe = 0;
char buffer[MAX_SIZE];
char ** arguments, ** arguments2, ** arguments3;

int token_counter(char * line, char * letter);
int command_counter(char * line, char letter);
char ** get_splited_args(char * com, char * letter);
void my_fork(char ** arguments);
void my_execute(char ** arguments);
void my_history(char * option, char * command);
// void mycd(char ** arguments);
void fatal(char * message, int code);



int main()
{
    // store standard fd
    s_stdin = dup(STDIN_FILENO);
    s_stdout = dup(STDOUT_FILENO);
    while (1)
    {
        int b, p;
        int command_number, background_number, pipe_number;
        char * com, * temp, * t_command;
        char command[MAX_SIZE] = {'\0'};
        char hostname[MAXHOSTNAMELEN] = {'\0'};
    
        // initialize vars
        idx = 0;
        ispipe = 0;
        isbackground = 0;
        
        // initialize standard fd
        dup2(s_stdin, STDIN_FILENO);
        dup2(s_stdout, STDOUT_FILENO);
        
        // get host name
        gethostname(hostname, MAXHOSTNAMELEN);
        
        // print shell prompt
        printf("%s@%s:%s >>> ", getenv("USER"), hostname, getenv("PWD"));
        
        fgets(command, MAX_SIZE, stdin);
        command[strlen(command)-1] = '\0';
        
        if (strlen(command) == 0)
            continue;
        
        if (strcmp(command, "exit") == 0)
            exit(0);
        
        if (strcmp(command, "history") == 0)
        {
            my_history("show", NULL);
            continue;
        }
        
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

            b = 0;
            background_number = command_counter(com, '&');
            arguments = get_splited_args(com, "&");
            for (int i = 0; *(arguments + i) != NULL; i++)
            {
                isbackground = 0;
                if (b++ < background_number)
                    isbackground = 1;
                p = 0;
                pipe_number = command_counter(*(arguments + i), '|');
                arguments2 = get_splited_args(*(arguments + i), "|");
                for (int j = 0; *(arguments2 + j) != NULL; j++)
                {
                    ispipe = 0;
                    if (p++ < pipe_number)
                        ispipe = 1;
                    arguments3 = get_splited_args(*(arguments2 + j), " ");
                    my_fork(arguments3);
                }
            }
        }
        // append command in history
        my_history("append", command);

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

void my_fork(char ** arguments)
{
    int status;
    int pid;
    if (ispipe)
    {
        pipe(fd);
//        fcntl(fd[0], fcntl(fd[0], F_SETPIPE_SZ, 1024 * 1024);
//        fcntl(fd[1], F_SETPIPE_SZ, 1024 * 1024);
    }
    

    if ((pid = fork()) < 0)
        fatal("Can not fork!", EXIT_FAILURE);
    
    // execute child process
    else if (pid == 0)
    {
        if (ispipe)
        {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
        }
        my_execute(arguments);
        exit(1);
    }
    // execute parent process
    else
    {
        if (ispipe)
        {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
        }
        if (isbackground)
        {
            printf("[%d] %d\n", ++idx, pid);
        }
        else
        {
            sleep(1);
            if((pid = waitpid(pid, &status, 0)) < 0)
                fatal("Can not wait!", 1);
            
        }
        if (ispipe)
        {
            close(fd[0]);
            close(fd[1]);
        }

    }
}

void my_execute(char ** arguments)
{
    execvp(* arguments, arguments);
}

void my_history(char * option, char * command)
{
    FILE * fp;
    char buffer[MAX_SIZE+10];
    if (strcmp(option, "show") == 0)
    {
        int idx = 0;
        if ((fp = fopen("./history.txt", "r")) == NULL)
            puts("No history");
        
        while(fgets(buffer, MAX_SIZE, fp))
        {
            printf(" %d  %s", ++idx, buffer);
            memset(buffer, '\0', MAX_SIZE);
        }
    }
    else
    {
        if ((fp = fopen("./history.txt", "a+")) == NULL)
            fatal("Can not open file!", EXIT_FAILURE);
        sprintf(buffer, "%s\n", command);
        if (fputs(buffer, fp) < 0)
            fatal("Can not write!", EXIT_FAILURE);
    }
    fclose(fp);
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
    

    // for storing
    strcpy(t_com, com);

    // for counting
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
