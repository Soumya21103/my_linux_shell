#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

// TO DO: IMPLEMENT ABSOLUTE PATHS see parent == 1 line
char PATH[PATH_MAX];
int mkdir_p(char** c,char* regex,int v)
{
    if(*c == NULL)
    {
        return 0;
    }
    DIR* dir = opendir(*c);
    int myern = errno;
    if (dir) {
        closedir(dir);
        chdir(*c);
        *c = strtok(NULL,regex);
        int ret =  mkdir_p(c,regex,v);
        chdir("..");
        return ret;

    } else if (ENOENT == myern) {
        mkdir(*c,0777);
        if(v == 1)
        {
            printf("mkdir: %s is created\n",*c);
        }
        chdir(*c);
        *c = strtok(NULL,regex);
        int ret =  mkdir_p(c,regex,v);
        chdir("..");
        return ret;

    } else if (ENOTDIR == myern) {
        return EEXIST;
    } else {
        return myern;
    }
}

int main(int argc, char const *argv[])
{
    int verb = 0;
    int parent = 0;

    for (int i = 0; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if(strcmp(argv[i],"-v") == 0)
            {
                verb = 1;
            } else if(strcmp(argv[i],"-p") == 0)
            {
                parent = 1;
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
        if(parent == 1)
        {
            //get cwd then check if argv[i][0] is '/' if yes then cd to root and cd back to cwd after everything is done.
            getcwd(PATH,sizeof(PATH));
            if(argv[i][0] == '/')
            {
                chdir("/");
            }
            char pth[PATH_MAX];
            strcpy(pth,argv[i]);
            char* c = strtok(pth,"/");
            if(c == NULL)
            {
                continue;
            }
            int check = mkdir_p(&c,"/",verb);
            if(check == 0)
            {
                continue;
            } else if(check == EEXIST)
            {
                printf("mkdir: %s is not a directory\n",c);
            } else {
                printf("mkdir: %s %s\n",c,strerror(check));
            }
            if(argv[i][0] == '/')
            {
                chdir(PATH);
            }
        } else {
            DIR* dir = opendir(argv[i]);
            if (dir) {
                closedir(dir);
                printf("mkdir: %s already exist\n",argv[i]);
            } else if (ENOENT == errno) {
                int check = mkdir(argv[i],0777);
                int myern = errno;
                if (check != 0)
                {
                    printf("mkdir: %s %s\n",argv[i],strerror(myern));
                    continue;
                }
                if(verb == 1)
                {
                    printf("mkdir: %s created\n",argv[i]);
                }
            } else {
                printf("mkdir: %s %s\n",argv[i],strerror(errno));
            }
        }
    }
    
    return 0;
}