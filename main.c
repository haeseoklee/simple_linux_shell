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

#define MAX_SIZE     1024
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define PERM         0777

int idx, s_stdin, s_stdout, s_sterr;
int fd[2], rfd, rfd2;
int isbackground = 0, ispipe = 0;
char **arguments0, ** arguments1, ** arguments2, ** arguments3, ** arguments4;

int token_counter(char * line, char * letter);
int command_counter(char * line, char * word);
char * command_replace(char * line, char * word, char * word2);
char ** get_splited_args(char * com, char * letter);
char ** get_bisected_args(char * com, char * letter);
void start(char * command);
void my_fork(char ** arguments);
void my_redirection(char * symbol, char * command);
char * my_history(char * option, char * command);
void my_cd(char ** arguments);
void my_set(char ** arguments);
void fatal(char * message, int code);
char * strip(char * word, char letter);
char * history_replace(char * command);
char * get_front_redirect(char * line);


int main()
{
    // store standard fd
    s_stdin = dup(STDIN_FILENO);
    s_sterr = dup(STDERR_FILENO);
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
        dup2(s_sterr, STDERR_FILENO);
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

void start(char * com)
{
    int b, p;
    int semicolon_num, background_num, pipe_num, exclamation_num;
    char * redirect_symbol, * t_command, * command;
    
    t_command = strdup(com);
    command = strdup(com);
    exclamation_num = command_counter(t_command, "!");
    if (exclamation_num > 0)
    {
        memset(command, '\0', strlen(command));
        command = history_replace(t_command);
        if (strcmp(command, t_command) != 0)
            puts(command);
    }
    
    my_history("append", command);
    
    semicolon_num = command_counter(command, ";");
    arguments0 = get_splited_args(command, ";");
    
    for (int i = 0; *(arguments0 + i) != NULL; i++)
    {
        // initialize standard fd
        dup2(s_stdin, STDIN_FILENO);
        dup2(s_sterr, STDERR_FILENO);
        dup2(s_stdout, STDOUT_FILENO);
        
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
                    
                    if (* (arguments3 + 1) == NULL)
                        * (arguments3 + 1) = "";
                    
                    char * symbols[6] = {">", "2>", ">>", "<", ">!"};
                    for (int l = 0; l < 5; l++)
                        if (!strcmp(redirect_symbol, symbols[l]))
                            my_redirection(redirect_symbol, *(arguments3 + 1));
                    
                    
                    arguments4 = get_splited_args(* arguments3, " ");
                    
                    if (* arguments4 == NULL)
                        puts("");
                    else if (!strcmp(* arguments4, "cd"))
                        my_cd(arguments4);
                    else if (!strcmp(* arguments4, "set"))
                        my_set(arguments4);
                    else if (!strcmp(* arguments4, "history"))
                        my_history("show", NULL);
                    else
                        my_fork(arguments4);
                    if (* (arguments3 + 1) == NULL)
                        break;
                    
                    redirect_symbol = get_front_redirect(*(arguments3 + 1));
                    arguments3 = get_bisected_args(*(arguments3 + 1), redirect_symbol);
                }
            }
        }
    }
}


void my_redirection(char * symbol, char * com)
{
    
    char * filename = strip(com, ' ');
    if (!strcmp(symbol, ">"))
    {
        FILE * fp;
        char buffer[MAX_SIZE];
        
        if (access(".myshellrc", F_OK) == -1)
        {
            if ((rfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, PERM)) < 0)
                fatal("Can not create file", EXIT_FAILURE);
            dup2(rfd, STDOUT_FILENO);
            close(rfd);
            return;
        }
        
        if ((fp = fopen(".myshellrc", "r")) == NULL)
            fatal("Can not read file", EXIT_FAILURE);
        
        fgets(buffer, MAX_SIZE, fp);
        
        if (!strcmp(buffer, "noclobber=-o\n"))
        {
            
            if (access(filename, F_OK) != -1)
            {
                fprintf(stderr, "myshell: %s: cannot overwrite existing file\n", filename);
                close(STDERR_FILENO);
                close(STDOUT_FILENO);
                fclose(fp);
                return;
            }
            else
            {
                if ((rfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, PERM)) < 0)
                    fatal("Can not create file", EXIT_FAILURE);
                dup2(rfd, STDOUT_FILENO);
                fclose(fp);
                close(rfd);
                return;
            }
        }
        if (!strcmp(buffer, "noclobber=+o\n") || !strlen(buffer))
        {
            if ((rfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, PERM)) < 0)
                fatal("Can not create file", EXIT_FAILURE);
            dup2(rfd, STDOUT_FILENO);
        }
        fclose(fp);
        close(rfd);
    }
    if (!strcmp(symbol, ">>"))
    {
       if ((rfd = open(filename, O_RDWR | O_CREAT | O_APPEND, PERM)) < 0)
           fatal("Can not create file", EXIT_FAILURE);
       dup2(rfd, STDOUT_FILENO);
       close(rfd);
    }
    if (!strcmp(symbol, ">!"))
    {
        if ((rfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, PERM)) < 0)
            fatal("Can not create file", EXIT_FAILURE);
        dup2(rfd, STDOUT_FILENO);
        close(rfd);
    }
    if (!strcmp(symbol, "2>"))
    {
        if ((rfd = open(filename, O_RDWR | O_CREAT | O_APPEND, PERM)) < 0)
            fatal("Can not create file", EXIT_FAILURE);
        dup2(rfd, STDERR_FILENO);
        close(rfd);
    }
    if (!strcmp(symbol, "<"))
   {
       if ((rfd = open(filename, O_RDONLY, PERM)) < 0)
           fatal("Can not create file", EXIT_FAILURE);
       dup2(rfd, STDIN_FILENO);
       close(rfd);
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
                fatal("Can not wait!", EXIT_FAILURE);
            
        }
        if (!ispipe)
        {
            close(fd[0]);
            close(fd[1]);
        }
    }
}


char * my_history(char * option, char * command)
{
    FILE * fp;
    char buffer[MAX_SIZE];
    memset(buffer, '\0', MAX_SIZE);
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
                fclose(fp);
                *(new_command + strlen(new_command)-1) = '\0';
                return new_command;
            }
            memset(buffer, '\0', MAX_SIZE);
        }
        fclose(fp);
        return command;
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

void my_set(char ** arguments)
{
    FILE * fp;
    char new_option[MAX_SIZE];
    if (*(arguments + 1) == NULL || *(arguments + 2) == NULL)
        return;
    if (!strcmp(*(arguments + 2), "noclobber"))
    {
        if ((fp = fopen("./.myshellrc", "w+")) == NULL)
            fatal("Can not open file!", EXIT_FAILURE);
        
        sprintf(new_option, "%s=%s\n", *(arguments + 2), *(arguments + 1));
        if (fwrite(new_option, 1, strlen(new_option), fp) < 0)
            fatal("Can not write!", EXIT_FAILURE);
        
        fclose(fp);
    }
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
    memset(new_word, '\0', MAX_SIZE);
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
    *(new_word + idx) = '\0';
    return new_word;
}

char * history_replace(char * com)
{
    char * comsave;
    char * command;
    char ** arguments;
    int exclamation_num;
    
    if ((comsave = (char *)malloc(MAX_SIZE * sizeof(char *))) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    
    if ((command = (char *)malloc(MAX_SIZE * sizeof(char *))) == NULL)
        fatal("Can not allocate memory!", EXIT_FAILURE);
    strcpy(command, com);
    
    memset(command, MAX_SIZE, '\0');
    memset(comsave, MAX_SIZE, '\0');
    
    START:
    arguments = get_splited_args(command, ";");
    
    for (int i = 0; *(arguments + i) != NULL; i++)
    {
        // if command includes "!" than search in history list
        exclamation_num = command_counter(*(arguments + i), "!");
        if (exclamation_num > 0)
        {
            memset(command, MAX_SIZE, '\0');
            command = strdup(my_history("shortcut", *(arguments + i)));
            
            if (!strcmp(command, *(arguments + i)))
                return command;
            for (;*(arguments + i) != NULL; i++)
                if (*(arguments + i + 1) != NULL)
                    sprintf(command, "%s;%s", command, *(arguments + i + 1));
            goto START;
        }
        if (i == 0)
        {
            memset(comsave, MAX_SIZE, '\0');
            strcpy(comsave, *(arguments + i));
        }
            
        else
            sprintf(comsave, "%s;%s", comsave, *(arguments + i));
    }
    return comsave;
}
