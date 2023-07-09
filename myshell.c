#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <assert.h>
#include <pthread.h>
// to test echo and cd -p everything else is working also if you can look at mkdir absolute pathing DONE
// error handling and provide test cases; make "" compactible with parsetext. add \ ' ' for this
void *invoke(void *args)
{
    int len = 0;
    int argc = 0;
    char **argv = (char **)args;
    while (argv[argc] != NULL)
    {
        len += strlen(argv[argc]) + 1;
        argc++;
    }
    char buffer[len];
    argc++;
    int pos = 0;
    for (int i = 0; i < argc - 1; i++)
    {
        int b = strlen(argv[i]);
        for (int j = 0; j < b; j++)
        {
            buffer[pos] = argv[i][j];
            pos++;
        }
        buffer[pos] = ' ';
        pos++;
    }
    buffer[len - 1] = '\0';
    system(buffer);
}

typedef struct LL
{
    char *data;
    struct LL *next;
} LL;

LL *create_node(char *data)
{
    LL *node = (LL *)malloc(sizeof(LL));
    node->data = (char *)malloc(sizeof(char) * PATH_MAX);
    strcpy(node->data, data);
    node->next = NULL;
    return node;
}

LL *add_new(char *data, LL *head)
{
    LL *node = (LL *)malloc(sizeof(LL));
    node->data = (char *)malloc(sizeof(char) * PATH_MAX);
    strcpy(node->data, data);
    node->next = head;
    return node;
}

LL *del_last(LL *head, char buffer[])
{
    if (head == NULL)
    {
        *buffer = '\0';
        return NULL;
    }
    LL *node = head->next;
    strcpy(buffer, head->data);
    free(head->data);
    free(head);
    return node;
}

typedef struct m_char_stream
{
    LL *head;
    int len;
    LL *ittr;
} CSTR;

CSTR *cstrinit()
{

    CSTR *node = (CSTR *)malloc(sizeof(CSTR));
    node->head = NULL;
    node->ittr = NULL;
    node->len = 0;
    return node;
}

void addstr(CSTR *cstr, char *string)
{
    if (cstr->head == NULL)
    {
        cstr->head = create_node(string);
    }
    else
    {
        cstr->head = add_new(string, cstr->head);
    }
    cstr->len += 1;
    cstr->ittr = cstr->head;
}

char *delstr(CSTR *cstr, char buffer[])
{
    if (cstr->head != NULL)
    {
        cstr->len -= 1;
    }
    cstr->head = del_last(cstr->head, buffer);
    cstr->ittr = cstr->head;
    return buffer;
}

char *readstr(CSTR *cstr, char buffer[])
{
    if (cstr->ittr == NULL)
    {
        return NULL;
    }
    strcpy(buffer, cstr->ittr->data);
    cstr->ittr = cstr->ittr->next;
}

void rewindstr(CSTR *cstr)
{
    cstr->ittr = cstr->head;
}

CSTR *deletecstr(CSTR *cstr)
{
    char buffer[PATH_MAX];
    while (cstr->head != NULL)
    {
        delstr(cstr, buffer);
    }
    free(cstr);
    return NULL;
}

int parse_path(CSTR *cstr, char *argv)
{
    char dump[256];
    char *p = strtok(argv, "/");
    while (p != NULL)
    {
        if (strlen(p) == 0)
        {
            p = strtok(NULL, "/");
            continue;
        }
        if (strcmp(p, "..") == 0)
        {
            delstr(cstr, dump);
        }
        else if (strcmp(p, ".") == 0)
        {
        }
        else
        {
            addstr(cstr, p);
        }
        p = strtok(NULL, "/");
    }
    return 0;
}

char *strstrip(char *s)
{
    unsigned long int size;
    char *end;
    size = strlen(s);
    if (!size)
    {
        return s;
    }
    end = s + size - 1;
    while (end >= s && (*end == ' ' || *end == '\n'))
    {
        end--;
    }
    *(end + 1) = '\0';
    while (*s && (*end == ' ' || *end == '\n'))
    {
        s++;
    }
    return s;
}

char cwd[PATH_MAX];
char HOME[PATH_MAX];
int flist_len = 0;
char PPATH[PATH_MAX];
char FPATH[PATH_MAX];
CSTR *apath;

int evaluate_path(CSTR *path, char buffer[])
{
    for (int i = 0; i < PATH_MAX; i++)
    {
        buffer[i] = 0;
    }
    int pos = 0;
    CSTR *holder = cstrinit();
    while (path->ittr != NULL)
    {
        char p[PATH_MAX];
        readstr(path, p);
        addstr(holder, p);
    }
    rewindstr(path);
    while (holder->head != NULL)
    {
        char p[PATH_MAX];
        delstr(holder, p);
        buffer[pos] = '/';
        pos++;
        int i = 0;
        while (p[i] != '\0')
        {
            buffer[pos] = p[i];
            i++;
            pos++;
        }
        buffer[pos] = '\0';
    }
    deletecstr(holder);
    return 0;
}

// internal funcs
int pwd(int argc, char *argv[])
{
    if (argc > 2)
    {
        return E2BIG;
    }
    else if (argc == 1)
    {
        char buffer[PATH_MAX];
        evaluate_path(apath, buffer);
        printf("%s\n", buffer);
    }
    else if (argc == 2 && strcmp(argv[1], "-P") == 0)
    {
        printf("%s\n", PPATH);
    }else if (argc == 2 && strcmp(argv[1], "-L") == 0)
    {
        char buffer[PATH_MAX];
        evaluate_path(apath, buffer);
        printf("%s\n", buffer);
    }
    else
    {
        return EINVAL;
    }
    return 0;
}

void setmycwd()
{
    char *id = NULL;

    while (id == NULL)
    {
        id = getcwd(PPATH, sizeof(PPATH));
    }
    id = NULL;
    while (id == NULL)
    {
        id = getenv("PWD");
    }
    setenv("PATH", id, 1);
    apath = cstrinit();

    char *p = strtok(id, "/");
    while (p != NULL)
    {
        addstr(apath, p);
        p = strtok(NULL, "/");
    }
}

int cd(int argc, char *argv[])
{

    if (argc == 1)
    {
        char *dir;
        dir = HOME;
        int check = chdir(dir);
        int myern = errno;
        if (check == 0)
        {
            char *id = NULL;

            while (id == NULL)
            {
                id = getcwd(PPATH, sizeof(PPATH));
            }
            apath = deletecstr(apath);
            apath = cstrinit();
            char pth[PATH_MAX];
            strcpy(pth,PPATH);
            char *p = strtok(pth, "/");
            while (p != NULL)
            {
                addstr(apath, p);
                p = strtok(NULL, "/");
            }
        }
        else
        {
            printf("cd: %s\n", strerror(myern));
            return -1;
        }
    }

    if (argc > 3)
    {
        return E2BIG;
    }
    else if (argc == 2)
    {
        int check;
        int myern;

        check = chdir(argv[1]);
        myern = errno;
        if (check == 0)
        {
            if (argv[1][0] == '/')
            {
                char *x = getcwd(PPATH, sizeof(PPATH));
                apath = deletecstr(apath);
                apath = cstrinit();
                char pth[PATH_MAX];
            strcpy(pth,PPATH);
            char *p = strtok(pth, "/");
                while (p != NULL)
                {
                    addstr(apath, p);
                    p = strtok(NULL, "/");
                }
            }
            else
            {
                char *x = getcwd(PPATH, sizeof(PPATH));
                while (x == NULL)
                {
                    x = getcwd(PPATH, sizeof(PPATH));
                }
                int y = parse_path(apath, argv[1]);
                char sympath[PATH_MAX];
                evaluate_path(apath, sympath);
                check = chdir(sympath);
                getcwd(PPATH, sizeof(PPATH));
                while (y != 0)
                {
                    y = parse_path(apath, argv[1]);
                }
            }
        }
        else
        {
            return myern;
        }
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-p") == 0)
        {
            /// @brief plz check the following argc == 3 argv[1] == -p
            /// @param dir
            /// @param argc
            /// @param argv
            /// @return

            int check = chdir(argv[2]);
            int myern = errno;
            if (check == 0)
            {
                char *x = getcwd(PPATH, sizeof(PPATH));
                apath = deletecstr(apath);
                apath = cstrinit();
                char pth[PATH_MAX];
            strcpy(pth,PPATH);
            char *p = strtok(pth, "/");
                while (p != NULL)
                {
                    addstr(apath, p);
                    p = strtok(NULL, "/");
                }
            }
            else
            {
                return myern;
            }
        }
        else if (strcmp(argv[1], "-l") == 0)
        {
            int check;
            int myern;

            check = chdir(argv[2]);
            myern = errno;
            if (check == 0)
            {
                if (argv[2][0] == '/')
                {
                    char *x = getcwd(PPATH, sizeof(PPATH));
                    apath = deletecstr(apath);
                    apath = cstrinit();
                    char *p = strtok(PPATH, "/");
                    while (p != NULL)
                    {
                        addstr(apath, p);
                        p = strtok(NULL, "/");
                    }
                }
                else
                {
                    char *x = getcwd(PPATH, sizeof(PPATH));
                    while (x == NULL)
                    {
                        x = getcwd(PPATH, sizeof(PPATH));
                    }
                    int y = parse_path(apath, argv[2]);
                    char sympath[PATH_MAX];
                    evaluate_path(apath, sympath);
                    check = chdir(sympath);
                    getcwd(PPATH, sizeof(PPATH));
                    while (y != 0)
                    {
                        y = parse_path(apath, argv[1]);
                    }
                }
            }
            else
            {
                return myern;
            }
        }
        else
        {
            return E2BIG;
        }
    }
    else if (argc == 1)
    {
        return 0;
    }
    return 0;
}

int echo(int argc, char *argv[])
{
    if (argc > 3)
    {
        return E2BIG;
    }
    else if (argc == 2)
    {
        printf("%s\n", argv[1]);
    }
    else if (argc == 1)
    {
        printf("\n");
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1],"-n") == 0)
        {
            printf("%s", argv[2]);
        }
        else if (strcmp(argv[1],"-e") == 0)
        {
            int flag = 0;
            int i = 0;
            char x = argv[2][i];
            while (x != '\0')
            {

                if (flag)
                {
                    if (x == 'n')
                    {
                        printf("\n");
                    }
                    else if (x == 't')
                    {
                        printf("\t");
                    }
                    else if (x == '\\')
                    {
                        printf("\\");
                    }
                    else if (x == 'a')
                    {
                        printf("\a");
                    }
                    else if (x == 'b')
                    {
                        printf("\b");
                    }
                    else if (x == 'e')
                    {
                        printf("\e");
                    }
                    else if (x == 'v')
                    {
                        printf("\v");
                    }else if (x == 's')
                    {
                        printf(" ");
                    }
                    else
                    {
                        printf("\\%c", x);
                    }
                    flag = 0;
                    i++;
                x = argv[2][i];
                    continue;
                }
                if (x == '\\')
                {
                    flag = 1;
                    i++;
                x = argv[2][i];
                    continue;
                }
                printf("%c", x);
                i++;
                x = argv[2][i];
            }
            printf("\n");
        }
        else
        {
            return EINVAL;
        }
    }
    return 0;
}

int parsetext(char *c)
{
    setenv("PATH", FPATH, 1);
    int ws = 0;
    /*
        "", " ","\n", "whitespaces", to be trimed beforehand, if fully empty dont call parse text
    */
    // LL* head = create_node(strtok(c," "));
    // int len = 1;
    // char* p = strtok(NULL," ");
    // while( p != NULL)
    // {
    //     head = add_new(p,head);
    //     len += 1;
    //     p = strtok(NULL," ");
    // }
    // int mlen = len;
    // char args[len];
    // while(head != NULL)
    // {
    //     args[len-1] = head -> data;
    //     len -= 1;
    // }
    CSTR *cstream = cstrinit();
    char *p = strtok(c, " ");
    while (p != NULL)
    {
        addstr(cstream, p);
        p = strtok(NULL, " ");
    }
    char args[cstream->len][4096];
    char *argvs[cstream->len + 1];
    for (int i = 0; i < cstream->len; i++)
    {
        argvs[i] = args[i];
    }
    argvs[cstream->len] = NULL;

    int len = cstream->len;
    char buff[4096];
    char *check = delstr(cstream, buff);
    while (strlen(check) != 0)
    {
        strcpy(args[cstream->len], buff);
        check = delstr(cstream, buff);
    }
    // replace everything with strcmp
    if (strcmp(args[0], "cd") == 0)
    {
        ws = cd(len, argvs) *256;
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        ws = pwd(len, argvs) *256;
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        ws = echo(len, argvs) *256;
    }
    else if (strcmp(args[0], "mkdir") == 0)
    {
        int rc = fork();
        if (rc < 0)
        {
            fprintf(stderr, "fork failed\n");
            exit(1);
        }
        else if (rc == 0)
        {
            execvp(argvs[0], argvs);
            printf("this shouldn’t print out");
        }
        else
        {

            int wc = wait(&ws);
        }
    }
    else if (strcmp(args[0], "cat") == 0)
    {
        int rc = fork();
        if (rc < 0)
        {
            fprintf(stderr, "fork failed\n");
            exit(1);
        }
        else if (rc == 0)
        {
            execvp(argvs[0], argvs);
            printf("this shouldn’t print out");
        }
        else
        {

            int wc = wait(&ws);
        }
    }
    else if (strcmp(args[0], "ls") == 0)
    {
        
        int rc = fork();
        if (rc < 0)
        {
            fprintf(stderr, "fork failed\n");
            ws = -2;
        }
        else if (rc == 0)
        {
            execvp(argvs[0], argvs);
            printf("this shouldn’t print out");
        }
        else
        {
            int wc = wait(&ws);
        }

    }
    else if (strcmp(args[0], "rm") == 0)
    {
        int rc = fork();
        if (rc < 0)
        {
            fprintf(stderr, "fork failed\n");
            exit(1);
        }
        else if (rc == 0)
        {
            execvp(argvs[0], argvs);
            printf("this shouldn’t print out");
        }
        else
        {

            int wc = wait(&ws);
        }
    }
    else if (strcmp(args[0], "date") == 0)
    {
        int rc = fork();
        if (rc < 0)
        {
            fprintf(stderr, "fork failed\n");
            exit(1);
        }
        else if (rc == 0)
        {
            execvp(argvs[0], argvs);
            printf("this shouldn’t print out");
        }
        else
        {
            int wc = wait(&ws);
        }
    }
    else if (strcmp(args[0], "mkdir&t") == 0)
    {
        argvs[0] = "mkdir";
        pthread_t pr;
        printf("hello\n");
        int rc = pthread_create(&pr, NULL, invoke, argvs);
        assert(rc == 0);
        rc = pthread_join(pr, NULL);
        assert(rc == 0);
    }
    else if (strcmp(args[0], "cat&t") == 0)
    {
        argvs[0] = "cat";
        pthread_t pr;
        int rc = pthread_create(&pr, NULL, invoke, argvs);
        assert(rc == 0);
        rc = pthread_join(pr, NULL);
        assert(rc == 0);
    }
    else if (strcmp(args[0], "ls&t") == 0)
    {
        argvs[0] = "ls";
        pthread_t pr;
        int rc = pthread_create(&pr, NULL, invoke, argvs);
        assert(rc == 0);
        rc = pthread_join(pr, NULL);
        assert(rc == 0);
    }
    else if (strcmp(args[0], "rm&t") == 0)
    {
        argvs[0] = "rm";
        pthread_t pr;
        int rc = pthread_create(&pr, NULL, invoke, argvs);
        assert(rc == 0);
        rc = pthread_join(pr, NULL);
        assert(rc == 0);
    }
    else if (strcmp(args[0], "date&t") == 0)
    {
        argvs[0] = "date";
        pthread_t pr;
        int rc = pthread_create(&pr, NULL, invoke, argvs);
        assert(rc == 0);
        rc = pthread_join(pr, NULL);
        assert(rc == 0);
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
    return ws;
}

int main(int argc, char const *argv[])
{
    strcpy(FPATH, getenv("PWD"));
    setmycwd();
    strcpy(HOME, getenv("HOME"));
    if (HOME == NULL)
    {
        printf("Something has went horibly wrong");
    }
    else
    {
        // printf("%s\n", HOME);
    }

    while (1)
    {
        char input[4 * 1024];
        char g;
        printf("\033[1;32mmyshell$\033[0;37m ");
        fgets(input, 4096, stdin);
        char *pinput = strstrip(input);
        int x;
        if (strlen(pinput))
        {
            x = parsetext(pinput);
        }
        if (x == -1)
        {
            break;
        } else if (x == -2)
        {

        }else if(x == 0) {

        } else {
            printf("myshell: %s\n",strerror(x/256));
        }
    }

    return 0;
}
