#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h> 
#include <fcntl.h> 
#include <sys/types.h>

struct Node
{
    char *word;
    struct Node *next;

    char *in;
    char *out;
    char *add;
    char *word1;
};

struct Node *addelem(struct Node *t, char *w)
{
    if (t == NULL)
    {
        t = (struct Node *)malloc(sizeof(struct Node));
        t->word = strdup(w);
        t->next = NULL;
    }
    else t->next = addelem(t->next, w);
    return t;
}

int list_size(struct Node *t)
{
    int size = 0;

    while (t != NULL)
    {
        size += 1;
        t = t->next;
    }

    return size;
}

void finprint(struct Node *t)
{
    if (t != NULL)
    {
        printf("%s\n", t->word);
        finprint(t->next);
    }
}

void freenode(struct Node *t)
{
    if (t != NULL)
    {
        freenode(t->next);
        free(t->word);
        free(t);
    }
}


int main(int argc, char **argv)
{
    FILE *fp1;
    fp1 = stdin;

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-i")) == 0)
        {
            if ((fp1 = fopen(argv[i + 1], "r")) == NULL)
            {
                fprintf(stderr, "Error:file doesn't open");
                return 0;
            }
        }
    }

    char c = 0;
    char *ctrl = ";<()&|>";
    char *ctrl2 = "&|>";

    int q = 0; // 0 не внутри кавычек, 1 - внутри кавычек
    int kq = 0;
    int step = 3;
    int exit = 1;
    while ((c != EOF)&&(exit==1))
    {

        char dir[PATH_MAX];
        getcwd(dir, sizeof(dir)); //копирует путь каталога, на который указывает dir
        printf("%s> ", dir);

        struct Node *t = NULL;
        int n = 0;
        char *w = (char *)malloc(sizeof(char) * step);
        while (((c = getc(fp1)) != '\n') && (c != EOF))
        {
            if (c == '"')
            {
                if (q == 1)
                    q = 0;
                else
                    q = 1;
                kq++;
            }
            else if ((isspace(c) && (q == 0))) //пробел вне кавычек
            {
                if (n > 0)
                {
                    w[n] = 0;
                    t = addelem(t, w);
                    n = 0;
                    free(w);
                    w = (char *)malloc(sizeof(char) * step);
                }
            }
            else if (strchr(ctrl, c) != NULL && (q == 0)) // спец. символ
            {
                if (n > 0)
                {
                    w[n] = 0;
                    t = addelem(t, w);
                    n = 0;
                    free(w);
                    w = (char *)malloc(sizeof(char) * step);
                }
                w[n] = c;
                n++;

                if (strchr(ctrl2, c) != NULL) // двойной спец.символ
                {
                    char b = getc(fp1);
                    if ((b != EOF) && (b == c))
                    {
                        w[n] = b;
                        n++;
                    }
                    else ungetc(b, fp1);
                }

                w[n] = 0;
                t = addelem(t, w);
                n = 0;
                free(w);
                w = (char *)malloc(sizeof(char) * step);
            }
            else if ((!isspace(c) || (isspace(c) && (q == 1))))
            {
                w[n] = c;
                n++;

                if (n == step)
                {
                    step = step * 2;
                    w = (char *)realloc(w, sizeof(char) * step);
                }
            }
        }

        w[n] = 0;
        t = addelem(t, w);
        n = 0;
        free(w);
        w = (char *)malloc(sizeof(char) * step);


        if (strcmp("cd", t->word) == 0)
        {
            if(t->next)
                chdir(t->next->word);
            else 
                chdir(getenv("HOME"));
        }
        else
        {
            int pid;
            if ((pid = fork()) < 0)
            {
                perror("cannot fork");
                return 1;
            }
            else if (!pid)
            {
                 /* child */
                struct Node *tt = t;
                char *argv[list_size(tt) + 1];
                int i;
                for (i = 0; tt != NULL; ++i)
                {
                    argv[i] = tt->word;
                    tt = tt->next;
                }
                argv[i] = NULL;

                execvp(*argv, argv);
                perror("exec");
            }
            else
            {
                /* parent */
                int status;
                while (wait(&status) > 0) continue;
            }
        }
        
        freenode(t);

        if (w != NULL) free(w);

        if ((kq % 2) == 1)
            fprintf(stderr, "Error:odd number of quotes\n");
    }

    fclose(fp1);
    return 0;
}
