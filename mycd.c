#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EXIT_FAILURE 1

void mycd(char ** arguments)
{
    char * path;
    if ((path = getcwd(NULL, 0)) == NULL)
    {
        perror("get path fail!");
        exit(EXIT_FAILURE);
    }
    else
    {
        setenv("OLDPWD", path, 1);
        chdir(arguments[1]);
        if ((path = getcwd(NULL, 0)) == NULL)
        {
            perror("get path fail!");
            exit(EXIT_FAILURE);
        }
        setenv("PWD", path, 1);
    }   
}