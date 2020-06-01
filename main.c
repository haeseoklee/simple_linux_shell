//
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
int isbackground = 0, ispipe = 0, isredirec1 = 0, isredirec2 = 0, isredirec3 = 0, isredirec4 = 0, isredirec5 = 0;
char **arguments0, ** arguments1, ** arguments2, ** arguments3, ** arguments4;

int token_counter(char * line, char * letter);
int command_counter(char * line, char * word);
char * command_replace(char * line, char * word, char * word2);
char ** get_splited_args(char * com, char * letter);
char ** get_bisected_args(char * com, char * letter);
void start(char * command);
void my_fork(char ** arguments);
char * my_history(char * option, char * command);
void my_cd(char ** arguments);
void fatal(char * message, int code);
char * strip(char * word, char letter);
char * history_replace(char * command);
char * get_front_redirect(char * line);


int main()
{
    // store standard fd
    s_stdin = dup(STDIN_FILENO);
    s_stdout = dup(STDOUT_FILENO);
    while (1)
    {
        char hostname[MAXHOSTNAMELEN] = {'\0'};
        char command[MAX_SIZE] = {'\0'};
    
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
        printf("%s@%s:%s 🔥🔥🔥 ", getenv("USER"), hostname, getcwd(NULL, 0));
        
        fgets(command, MAX_SIZE, stdin);
        command[strlen(command)-1] = '\0';
        
        if (!strlen(command))
            continue;
        
        start(command);
    }
}

void start(char * command)
{
    int b, p;
    int semicolon_num, background_num, pipe_num, exclamation_num;
    char * redirect_symbol;
    
    
    exclamation_num = command_counter(command, "!");
    if (exclamation_num > 0)
    {
        command = history_replace(command);
        puts(command);
    }
    
    my_history("append", command);
    
    semicolon_num = command_counter(command, ";");
    arguments0 = get_splited_args(command, ";");
    
    for (int i = 0; *(arguments0 + i) != NULL; i++)
    {
        b = 0;
        background_num = command_counter(*(arguments0 + i), "&");
        arguments1 = get_splited_args(*(arguments0 + i), "&");
        for (int j = 0; *(arguments1 + j) != NULL; j++)
        {
            isbackground = 0;
            if (b++ < background_num)
                isbackground = 1;
            
            *(arguments1 + j) = command_replace(*(arguments1 + j), ">|", ">!");
            
            p = 0;
            pipe_num = command_counter(*(arguments1 + j), "|");
            arguments2 = get_splited_args(*(arguments1 + j), "|");
            for (int k = 0; *(arguments2 + k) != NULL; k++)
            {
                ispipe = 0;
                if (p++ < pipe_num)
                    ispipe = 1;
                
                redirect_symbol = get_front_redirect(*(arguments2 + k));
                arguments3 = get_bisected_args(*(arguments2 + k), redirect_symbol);
                
                for (;;)
                {
                    if (* (arguments3) == NULL)
                        break;
                    
                    if (!strcmp(redirect_symbol, ">"))
                        isredirec1 = 1;
                    else if (!strcmp(redirect_symbol, "2>"))
                        isredirec2 = 1;
                    else if (!strcmp(redirect_symbol, ">>"))
                        isredirec3 = 1;
                    else if (!strcmp(redirect_symbol, "<"))
                        isredirec4 = 1;
                    else if (!strcmp(redirect_symbol, ">!"))
                        isredirec5 = 1;
                    
                    arguments4 = get_splited_args(* arguments3, " ");
                    if (* arguments4 == NULL)
                        puts("");
                    else if (!strcmp(* arguments4, "cd"))
                        my_cd(arguments4);
                    else if (!strcmp(* arguments4, "history"))
                        my_history("show", NULL);
                    else
                        my_fork(arguments4);

                    if (* (arguments3 + 1) == NULL)
                        break;
                    
                    redirect_symbol = get_front_redirect(* (arguments3 + 1));
                    arguments3 = get_bisected_args(* (arguments3 + 1), redirect_symbol);
                }
            }
        }
    }
}


char * get_front_redirect(char * com)
{
    char * letter = "";
    char * line;
    int flag = 0;
    
    if ((line = strdup(com)) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    
    if (line == NULL)
        return "";
    else
    {
        size_t linelen = strlen(line);
        int i;
        
        for(i = 0; i < linelen; i++)
            if (line[i] == '>' || line[i] == '<' || line[i] == '2')
            {
                flag = 1;
                break;
            }
        
        if (!flag)
            return "";
            
        
        if (&line[i+1] != NULL)
        {
            if (line[i] == '>')
            {
                if (line[i+1] == '>')
                    letter = strdup(">>");
                else if (line[i+1] == '!')
                    letter = strdup(">!");
                else
                    letter = strdup(">");
            }
            if (line[i] == '2')
                if (line[i+1] == '>')
                    letter = strdup("2>");
            if (line[i] == '<')
                letter = strdup("<");
        }
        else
        {
            if (line[i] == '>')
                letter = strdup(">");
            else if (line[i] == '<')
                letter = strdup("<");
            else
                letter = strdup("");
        }
        return letter;
    }
    return "";
}



char * command_replace(char * line, char * word, char * word2)
{
    if (line == NULL || word == NULL || word2 == NULL)
        return "";
    int flag;
    size_t linelen = strlen(line);
    size_t wordlen = strlen(word);
    size_t word2len = strlen(word2);
    for (int i = 0; i < linelen; i++)
    {
        flag = 1;
        for (int j = 0; j < wordlen; j++)
        {
            if (i+j >= linelen)
            {
                flag = 0;
                break;
            }
            if (line[i+j] != word[j])
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            for (int j = 0; j < word2len; j++)
            {
                if (i+j >= linelen)
                    break;
                line[i+j] = word2[j];
            }
        }
    }

    return line;
}


int command_counter(char * line, char * word)
{
    if (line == NULL || word == NULL)
        return 0;
    int counter = 0;
    int flag;
    size_t linelen = strlen(line);
    size_t wordlen = strlen(word);
    
    for (int i = 0; i < linelen; i++)
    {
        flag = 1;
        for (int j = 0; j < wordlen; j++)
        {
            if (i+j >= linelen)
            {
                flag = 0;
                break;
            }
            if (line[i+j] != word[j])
            {
                flag = 0;
                break;
            }
        }

        if (flag)
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
        execvp(* arguments, arguments);
        // 파일을 만들거나 불러와서 처리함
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


char * my_history(char * option, char * command)
{
    FILE * fp;
    char buffer[MAX_SIZE+10];
    if (!strcmp(option, "show"))
    {
        
        int idx = 0;
        if ((fp = fopen("./.history", "r")) == NULL)
            fatal("Can not open file", EXIT_FAILURE);
        
        while(fgets(buffer, MAX_SIZE, fp))
        {
            printf(" %d  %s", ++idx, buffer);
            memset(buffer, '\0', MAX_SIZE);
        }
    }
    else if (!strcmp(option, "shortcut"))
    {
        int idx = 0;
        char * new_command, * num, * com;
        
        if ((com = strdup(command)) == NULL)
            fatal("Can not allocate memory!", EXIT_FAILURE);
        
        com = strip(com, ' ');
        num = strtok(com, "!");
        
        if ((fp = fopen("./.history", "r")) == NULL)
            fatal("Can not open file", EXIT_FAILURE);
        
        while(fgets(buffer, MAX_SIZE, fp))
        {
            if (num == NULL)
                break;
            if (++idx == atoi(num))
            {
                if ((new_command = strdup(buffer)) == NULL)
                    fatal("Can not allocate memory!", EXIT_FAILURE);
                * (new_command + strlen(new_command)-1) = '\0';
                return new_command;
            }
            memset(buffer, '\0', MAX_SIZE);
        }
    }
    else
    {
        if ((fp = fopen("./.history", "a+")) == NULL)
            fatal("Can not open file!", EXIT_FAILURE);
        sprintf(buffer, "%s\n", command);
        if (fputs(buffer, fp) < 0)
            fatal("Can not write!", EXIT_FAILURE);
    }
    fclose(fp);
    return "";
}

void my_cd(char ** arguments)
{
    char * path;
    if (*(arguments + 1) == NULL)
        chdir(getenv("HOME"));
    else
        chdir(*(arguments + 1));
    if ((path = getcwd(NULL, 0)) == NULL)
        fatal("Can not get path", EXIT_FAILURE);
}

void fatal(char * message, int code)
{
    perror(message);
    exit(code);
}

char ** get_bisected_args(char * com, char * letter)
{
    
    char ** arguments;
    char * t_com;
    
    if ((t_com = strdup(com)) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    
    if ((arguments = (char **)malloc(3 * sizeof(char *))) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    
    *arguments = strtok(t_com, letter);
    if (*arguments != NULL)
    {
        *(arguments + 1) = strtok(NULL, "");
        if ( *(arguments + 1) != NULL)
            for (int i = 0; i < strlen(letter)-1; i++)
                *(*(arguments + 1) + i) = ' ';
    }
    else
         *(arguments + 1) = NULL;
    *(arguments + 2) = NULL;
    
    return arguments;
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

char * strip(char * word, char letter)
{
    char * new_word;
    int idx = 0;
    int start = 0, end = (int)(strlen(word)-1);
    if ((new_word = (char *)malloc(MAX_SIZE)) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    for(int i = 0; i < strlen(word); i++)
    {
        if (word[i] == letter)
            start++;
        else
            break;
    }
    for(int i = (int)(strlen(word)-1); i >= 0; i--)
    {
        if (word[i] == letter)
            end--;
        else
            break;
    }
    for (int i = start; i <= end; i++)
        new_word[idx++] = word[i];
    return new_word;
}

char * history_replace(char * command)
{
    char * comsave;
    char ** arguments;
    int exclamation_num;
    
    if ((comsave = (char *)malloc(MAX_SIZE * sizeof(char *))) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    
    memset(comsave, MAX_SIZE, '\0');
    
    START:
    arguments = get_splited_args(command, ";");
    
    for (int i = 0; *(arguments + i) != NULL; i++)
    {
        // if command includes "!" than search in history list
        exclamation_num = command_counter(*(arguments + i), "!");
        if (exclamation_num > 0)
        {
            command = my_history("shortcut", *(arguments + i));
            for (;*(arguments + i) != NULL; i++)
                if (*(arguments + i + 1) != NULL)
                    sprintf(command, "%s;%s", command, *(arguments + i + 1));
            goto START;
        }
        if (strlen(comsave) == 0)
            strcpy(comsave, *(arguments + i));
        else
            sprintf(comsave, "%s;%s", comsave, *(arguments + i));
    }
    return comsave;
}
