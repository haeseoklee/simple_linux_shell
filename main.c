#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>

#define MAX_SIZE     1000
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

int main()
{
    while (1)
    {
        int n = 0;
        int m = 0;
        int c;
        int command_counter = 1;
        int command_token_counter = 1;
        char * t_command;
        char * com;
        char * tok;
        char * t_com;
        char * tt_com;
        char ** command_table;
        char command[MAX_SIZE] = {'\0'};
        char hostname[MAXHOSTNAMELEN] = {'\0'};
        int h = gethostname(hostname, MAXHOSTNAMELEN);
        if (h) 
        {
            puts("Can not get hostname");
            exit(EXIT_FAILURE);
        }
        printf("%s@%s:%s >>> ", (char *)getenv("USER"), hostname, (char *)getenv("PWD"));
        
        while ((c = fgetc(stdin)))
        {
            if (c == 10)
            {
                if ((t_command = (char *)malloc(strlen(command) + 1)) == NULL)
                {
                    puts("Can not allocate memory!");
                    exit(EXIT_FAILURE);
                }
                // TODO: add command in history 
                strcpy(t_command, command);
                if (strlen(t_command) == 0)
                    break;
                
                // count number of the commands
                if ((com = strtok(t_command, ";")) != NULL)
                {
                    while (1)
                    {
                        if ((com = strtok(NULL, ";")) == NULL)
                            break;
                        else
                            command_counter++;
                    }
                }
                while (m < command_counter)
                {
                    strcpy(t_command, command);
                    com = strtok(t_command, ";");
                    for (int i = 0; i < m; i++)
                        com = strtok(NULL, ";");
                    if ((t_com = (char *)malloc(strlen(com) + 1)) == NULL)
                    {
                        puts("Can not allocate memory!");
                        exit(1);
                    }
                    if ((tt_com = (char *)malloc(strlen(com) + 1)) == NULL)
                    {
                        puts("Can not allocate memory!");
                        exit(1);
                    }
                    strcpy(t_com, com);
                    strcpy(tt_com, com);

                    // count the number of tokens in one command
                    command_token_counter = 1;
                    if ((tok = strtok(tt_com, " ")) != NULL)
                    {
                        while (1)
                        {
                            if ((tok = strtok(NULL, " ")) == NULL)
                                break;
                            else
                                command_token_counter++;
                        }
                    }


                    puts(t_com);
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