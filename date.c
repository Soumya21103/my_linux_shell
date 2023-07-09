#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

int getfiletime(const char* c, int U)
{
    struct stat fs;
    int check = stat(c,&fs);
    int myern = errno;
    if (check != 0)
    {
        return myern;
    }
    struct tm* caltime;
    if (U) {
        caltime = gmtime(&(fs.st_atime));
        myern = errno;
        if(caltime == NULL)
        {
            return myern;
        }
    } else {
        caltime = localtime(&(fs.st_atime));
        myern = errno;
        if(caltime == NULL)
        {
            return myern;
        }
    }
    char buffer[128];
    strftime(buffer,128,"%c %Z", caltime);
    printf("%s\n",buffer);
    return 0;
}

int getnow(int U)
{
    time_t curt = time(NULL);
    int myern;
    struct tm* caltime;
    if (U) {
        caltime = gmtime(&(curt));
        myern = errno;
        if(caltime == NULL)
        {
            return myern;
        }
    } else {
        caltime = localtime(&(curt));
        myern = errno;
        if(caltime == NULL)
        {
            return myern;
        }
    }
    char buffer[128];
    strftime(buffer,128,"%c %Z", caltime);
    printf("%s\n",buffer);
    return 0;

}
int main(int argc, char const *argv[])
{
    int U = 0; int R = 0;
    for (int i = 0; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if(strcmp(argv[i],"-r") == 0)
            {
                R = 1;
            } else if (strcmp (argv[i],"-u") == 0)
            {
                U = 1;
            } else {
                continue;
            }
            argv[i] = NULL;
        }
    }
    argv[0] = NULL;
    if(R)
    {
        for (int i = 0; i < argc; i++)
        {
            if(argv[i] == NULL)
            {
                continue;
            }
            printf("date: %s : ",argv[i]);
            int check = getfiletime(argv[i],U);
            if (check != 0)
            {
                printf("%s\n",strerror(check));
            }
        }
        
    } else {
        printf("date: ");
        int check = getnow(U);
        if (check != 0)
            {
                printf("%s\n",strerror(check));
            }
    }
    
    return 0;
}
