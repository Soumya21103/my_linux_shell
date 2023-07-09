#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

char cwd[PATH_MAX];

// char* id = NULL;
//     while(id == NULL)
//     {
//         id = getcwd(cwd, sizeof(cwd));
//     }
//     DIR* d = opendir(cwd);
//     struct dirent* f = readdir(d);
//     while(f != NULL)
//     {
//         if(f->d_name[0] == '.')
//         {
//             f = readdir(d);
//             continue;
//         }
//         if(f->d_type == DT_DIR)
//         {
//             printf("\033[1;36m%s  ",f->d_name);
//         }
//         printf("\033[0;37m%s  ",f->d_name);
//         f = readdir(d);
//     }
//     printf("\n");
//     return 0;


void printdir(struct dirent* p)
{
    switch (p->d_type)
    {
    case DT_BLK:
        printf("\033[1;30m%s\033[0;34m  ",p->d_name);
        break;
    case DT_CHR:
        printf("\033[1;31m%s\033[0;34m  ",p->d_name);
        break;
    case DT_DIR:
        printf("\033[1;32m%s\033[0;34m  ",p->d_name);
        break;
    case DT_FIFO:
        printf("\033[1;33m%s\033[0;34m  ",p->d_name);
        break;
    case DT_LNK:
        printf("\033[1;34m%s\033[0;34m  ",p->d_name);
        break;
    case DT_REG:
        printf("\033[1;35m%s\033[0;34m  ",p->d_name);
        break;
    case DT_SOCK:
        printf("\033[1;36m%s\033[0;34m  ",p->d_name);
        break;
    case DT_UNKNOWN:
        printf("\033[1;37m%s\033[0;34m  ",p->d_name);
        break;
    default:
        printf("\033[0;34m%s\033[0;34m  ",p->d_name);
        break;
    }


}

void printdiri(struct dirent* p)
{
    switch (p->d_type)
    {
    case DT_BLK:
        printf("\033[1;30m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    case DT_CHR:
        printf("\033[1;31m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    case DT_DIR:
        printf("\033[1;32m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    case DT_FIFO:
        printf("\033[1;33m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    case DT_LNK:
        printf("\033[1;34m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    case DT_REG:
        printf("\033[1;35m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    case DT_SOCK:
        printf("\033[1;36m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    case DT_UNKNOWN:
        printf("\033[1;37m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    default:
        printf("\033[0;36m%lu %s\n\033[0;34m",p ->d_ino,p->d_name);
        break;
    }

}
int main(int argc, char const *argv[])
{
    
    char* id = NULL;
    while(id == NULL)
    {
        id = getcwd(cwd, sizeof(cwd));
    }

    if(argc > 3)
    {
        exit(E2BIG);
    } else if(argc == 1)
    {
        DIR* d = opendir(cwd);
        struct dirent* p = readdir(d);
        while(p != NULL)
        {
            if(p ->d_name[0] == '.')
            {
                p = readdir(d);
                continue;
            }
            printdir(p);
            p = readdir(d);
        }
        printf("\n");
    } else if(argc == 2)
    {
        if(argv[1][0] == '-')
        {
            if (strcmp(argv[1],"-a") == 0)
            {
                
                DIR* d = opendir(cwd);
                struct dirent* p = readdir(d);
                while(p != NULL)
                {
                    printdir(p);
                    p = readdir(d);
                }
                printf("\n");
            } else if(strcmp(argv[1],"-i") == 0)
            {
                DIR* d = opendir(cwd);
                struct dirent* p = readdir(d);
                while(p != NULL)
                {
                    printdiri(p);
                    p = readdir(d);
                }
                
            } else {
                exit(ENOTDIR);
            }
            
        } else {
            DIR* d = opendir(argv[1]);
            int myern = errno;
            if(d == NULL)
            {
                exit(myern);
            }
            struct dirent* p = readdir(d);
            while(p != NULL)
            {
                if(p ->d_name[0] == '.')
                {
                    p = readdir(d);
                    continue;
                }
                printdir(p);
                p = readdir(d);
            }
            printf("\n");
        }
    } else if(argc == 3)
    {
        if(argv[1][0] == '-')
        {
            if (strcmp(argv[1],"-a") == 0)
            {
                DIR* d = opendir(argv[2]);
                if(d == NULL)
                {
                    exit(ENOTDIR);
                }
                struct dirent* p = readdir(d);
                while(p != NULL)
                {
                    printdir(p);
                    p = readdir(d);
                }
                printf("\n");
            } else if(strcmp(argv[1],"-i") == 0)
            {
                DIR* d = opendir(argv[2]);
                if(d == NULL)
                {
                    exit(ENOTDIR);
                }
                struct dirent* p = readdir(d);
                while(p != NULL)
                {
                    printdiri(p);
                    p = readdir(d);
                }
            } else {
                exit(E2BIG);
            }
            
        } else {
            exit(E2BIG);
        }
    }
    exit(0);
}