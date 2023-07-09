#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

int rem(char* d,int pmt)
{
    DIR* dir = opendir(d);
    int myern = errno;
    if (dir) {
        chdir(d);
        struct dirent* anchor = readdir(dir);
        while(anchor != NULL)
        {
            if(anchor->d_type == DT_DIR)
            {
                if (anchor->d_name[0] == '.')
                {
                    // I've started believing in God now.
                    anchor = readdir(dir);
                    continue;
                }
                
                int check = rem(anchor->d_name,pmt);
                if(check != 0)
                {
                    chdir("..");
                    closedir(dir);
                    return check;
                }
                if(pmt == 1)
                {
                    char ans;
                    char dump;
                    printf("rm: %s will be removed? ",anchor->d_name);
                    scanf("%c",&ans);
                    scanf("%c",&dump);
                    if(!(ans == 'y' || ans =='Y'))
                    {
                        anchor = readdir(dir);
                        continue;
                    }
                }
                check = remove(anchor->d_name);
                myern = errno;
                if(check != 0)
                {
                    chdir("..");
                    closedir(dir);
                    return myern;
                }
            } else {
                if(pmt == 1)
                {
                    char ans;
                    char dump;
                    printf("rm: %s will be removed? ",anchor->d_name);
                    scanf("%c",&ans);
                    scanf("%c",&dump);
                    if(!(ans == 'y' || ans =='Y'))
                    {
                        anchor = readdir(dir);
                        continue;
                    }
                }
                int check = remove(anchor->d_name);
                myern = errno;
                if(check != 0)
                {
                    chdir("..");
                    closedir(dir);
                    return myern;
                }
            }
            anchor = readdir(dir);
        }
        chdir("..");
        closedir(dir);
        return 0;
    } else if (ENOENT == myern) {
        return myern;
    } else {
        return myern;
    }
}

int rm(char* c,int r, int p)
{
    if(r)
    {
        DIR* dir = opendir(c);
        int myern = errno;
        if(dir != NULL)
        {
            closedir(dir);
            int ret = rem(c,p);
            if(p == 1)
            {
                char ans;
                printf("rm: %s will be removed? ",c);
                scanf("%c",&ans);
                char dump;
                scanf("%c",&dump);
                if(!(ans == 'y' || ans =='Y'))
                {
                    return 0;
                }
            }
            int check = remove(c);
            myern = errno;
            if(check != 0)
            {
                return myern;
            }
            return ret;
        } else {
            if(p == 1)
            {
                char ans;
                char dump;
                printf("rm: %s will be removed? ",c);
                scanf("%c",&ans);
                scanf("%c",&dump);
                if(!(ans == 'y' || ans =='Y'))
                {
                    return 0;
                }
            }

            int check = remove(c);
            myern = errno;
            if(check != 0)
            {
                return myern;
            }
        }
    } else {
        if(p == 1)
        {
            char ans;
            char dump;
            printf("rm: %s will be removed? ",c);
            scanf("%c",&ans);
            scanf("%c",&dump);
            if(!(ans == 'y' || ans =='Y'))
            {
                return 0;
            }
        }
        int check = unlink(c);
        int myern = errno;
        if(check != 0)
        {
            return myern;
        }
    }
}

int main(int argc, char const *argv[])
{
    int prompt = 0;
    int recursive = 0;
    for (int i = 0; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if (strcmp(argv[i],"-i") == 0)
            {
                prompt = 1;
            } else if (strcmp(argv[i],"-r") == 0)
            {
                recursive = 1;
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
        char c[PATH_MAX];
        char* ch = strcpy(c,argv[i]);
        if(ch == NULL)
        {
            printf("Something went horibly wrong");
        }
        char cwd[PATH_MAX];
        if(getcwd(cwd,sizeof(cwd)) == NULL)
        {
            printf("cwd is not working. Does shell have permissions to be executed?");
            return -1;
        }
        int check = rm(c,recursive,prompt);
        if(c[0] == '/')
        {
            chdir(cwd);
        }
        if(check != 0)
        {
            printf("rm: %s %s\n",argv[i],strerror(check));
        }
    }
    return 0;
}
