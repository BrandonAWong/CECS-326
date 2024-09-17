/**
 * filecopy.c
 * 
 * This program copies files using a pipe.
 *
 */

#include <complex.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1

char *getFileContent(int file)
{
    size_t size = 10;
    char *buf = malloc(size);
    FILE *stream = fdopen(file, "r");
    int i = 0;
    int chr;
    while ((chr = fgetc(stream)) != EOF)
    {
         if (i >= size)
         {
            size *= 2;
            buf = realloc(buf, size);
            if (buf == NULL)
            {
                printf("Ran out of memory");
                exit(EXIT_FAILURE);
            }
         }
         buf[i++] = chr;
    }
    buf[i] = '\0';
    fclose(stream);
    return buf;
}

void writeFile(int infile, int outfile)
{
    char *content = getFileContent(infile);
    write(outfile, content, strlen(content));
    free(content);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("wrong amount of arguments ratard.\n");
        return EXIT_FAILURE;
    }
    const char *inpath = argv[1];
    const char *outpath = argv[2];

    int infile = open(inpath, O_RDONLY);
    if (infile < 0)
    {
        printf("Cannot copy '%s': No such file or directory\n", inpath);
        return EXIT_FAILURE;
    }

    int outfile = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfile < 0)
    {
        printf("Cannot create regular file '%s': No such file or directory\n", outpath);
        return EXIT_FAILURE;
    }

    int pipefd[2];  
    pipe(pipefd); 

    pid_t pid = fork();
    if (pid > (pid_t) 0)
    {
        // parent
        // reading source and writing to pipe
        close(pipefd[READ_END]);
        writeFile(infile, pipefd[WRITE_END]);
        close(pipefd[WRITE_END]);
        close(infile);
    }
    else if (pid == (pid_t) 0)
    {
        // child reading pipe and writing to destination
        close(pipefd[WRITE_END]);
        writeFile(pipefd[READ_END], outfile);
        close(pipefd[READ_END]);
        close(outfile);
    }
    else 
    {
        printf("Fork failed");
        close(infile);
        close(outfile);
        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

