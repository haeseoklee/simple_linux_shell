#include <stdio.h>
#include <string.h>

int main() 
{
    char * ptr;
    char string [] = "ls&echo&whoami";
    ptr = strtok(string, "&");
    for (int i = 0; i < 1; i++)
        ptr = strtok(NULL, "&");
    puts(ptr);
    return 0;
}