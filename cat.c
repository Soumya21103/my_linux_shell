#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

int nutp(const char* fil,int E,int N){
    FILE* ptr;
    char ch;
 

    ptr = fopen(fil, "r");
 
    if (NULL == ptr) {
        return -1;
    }

    int index = 1;
    if (N == 1)
    {    
        printf("%d ",index);
    }

    do {
        ch = fgetc(ptr);
        if(ch == '\n' && E == 1){
            printf("%c",'$');
        }
        printf("%c", ch);
        if(ch == '\n'&& N == 1){
            index += 1;
            printf("%d ",index);
        }
    } while (ch != EOF);
    printf("\n");
    fclose(ptr);
    return 0;
}

int main(int argc, char const *argv[])
{
    int E = 0; int N = 0;
    for (int i = 0; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if(argv[i][1] == 'E')
            {
                E = 1;
            } else if (argv[i][1] == 'n')
            {
                N = 1;
            } else {
                continue;
            }
            argv[i] = NULL;
        }
        
    }
    argv[0] = NULL;
    for (int i = 0; i < argc; i++)
    {
        if(argv[i] == NULL)
        {
            continue;
        }
        if(nutp(argv[i],E,N) == -1)
    {
        printf("file cant be opened \n");
    }
    }   
}
