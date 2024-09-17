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

// Read and return plaintext of a file descriptor
// Return NULL if system runs out of memory
char *getFileContent(int file)
{
    size_t size = 64;
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
            // check if memory is ok
            if (buf == NULL)
            {
                return NULL;
            }
         }
         buf[i++] = chr;
    }
    buf[i] = '\0';
    fclose(stream);
    return buf;
}

// Copy the content of infile into the outfile
// Return 0 on success, 1 on failure
int copyFile(int infile, int outfile)
{
    char *content = getFileContent(infile);
    if (content == NULL)
    {
        return 1;
    }
    write(outfile, content, strlen(content));
    free(content);
    return 0;
}

int main(int argc, char *argv[])
{
    // verify arguments
    if (argc != 3)
    {
        printf("Error: wrong amount of arguments (filecopy SOURCE DESTINATION)\n");
        return EXIT_FAILURE;
    }
    const char *inpath = argv[1];
    const char *outpath = argv[2];

    // verify source and destination are valid
    int infile = open(inpath, O_RDONLY);
    if (infile < 0)
    {
        printf("Error: Cannot copy '%s': No such file or directory\n", inpath);
        return EXIT_FAILURE;
    }

    int outfile = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfile < 0)
    {
        close(infile);
        printf("Error: Cannot create regular file '%s': No such file or directory\n", outpath);
        return EXIT_FAILURE;
    }

    int pipefd[2];
    // create and verify pipe
    if (pipe(pipefd) < 0)
    {
        printf("Error: Pipe failed");
        close(infile);
        close(outfile);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid > (pid_t) 0)
    {
        // parent process
        // reading source and writing to pipe
        close(pipefd[READ_END]);
        if (copyFile(infile, pipefd[WRITE_END]) > 0)
        {
            close(infile);
            close(outfile);
            close(pipefd[WRITE_END]);
            printf("Error: Ran out of memory\n");
            return EXIT_FAILURE;
        }
        close(pipefd[WRITE_END]);
        close(infile);
    }
    else if (pid == (pid_t) 0)
    {
        // child process
        // reading pipe and writing to destination
        close(pipefd[WRITE_END]);
        if (copyFile(pipefd[READ_END], outfile) > 0)
        {
            close(infile);
            close(outfile);
            close(pipefd[READ_END]);
            printf("Error: Ran out of memory\n");
            return EXIT_FAILURE;
        }
        close(pipefd[READ_END]);
        close(outfile);
        printf("File successfully copied from %s to %s\n", inpath, outpath);
    }
    else 
    {
        printf("Error: Fork failed\n");
        close(infile);
        close(outfile);
        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

