/************************************************************************* 
 * 12/31/2016                                                            *
 * prompt.c was downloaded from MTU:                                      *
 * http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html     *
 *                                                                       *
 * NOTE: The original code has quite ugly parse() routine with dangerous *
 * library routine gets(). To address these deficiences, I (Gang-Ryung)  *
 * rewrote main() and parse() routine using fgets and strtok.            *
 * UPDATE: Revision made by AUTHOR: Chris Georgiades. I have added the 
 * batchmode functionality as well as command piping. The  
 * 
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

/* ----------------------------------------------------------------- */
/* FUNCTION  parse:                                                  */
/* This function takes an input line and parse it into tokens.       */
/* The delimiter for strtok must be passed as an argument.           */
/* It saves the address to argv[], and then skips all                */
/* non-white spaces which constitute the argument.                   */
/* ----------------------------------------------------------------- */

size_t parse(char *line, char **argv, char *delimiter)
{
     char *token = strtok(line, delimiter);
     size_t count = 0;
     while (token != NULL)
     {
          *argv++ = token;
          ++count;
          token = strtok(NULL, delimiter);
     }
     *argv = (char *)'\0'; /* mark the end of argument list  */
     return count;
}

/* ----------------------------------------------------------------- */
/* FUNCTION execute:                                                 */
/*    This function receives a commend line argument list with the   */
/* first one being a file name followed by its arguments.  Then,     */
/* this function forks a child process to execute the command using  */
/* system call execvp().                                             */
/* ----------------------------------------------------------------- */

void execute(char **args)
{
     pid_t pid, c;
     int status;

     if ((pid = fork()) == 0)
     {
          // child process
          if (strcmp(args[0], "quit") == 0) // check for quit command
               exit(0);

          execvp(args[0], args); /* arg[0] has the command name. */

          fprintf(stderr, "Exec failed on command %s.\n", *args);
          exit(1);
     }
     else
     {
          if (pid == (pid_t)(-1))
          {
               fprintf(stderr, "Fork failed.\n");
               exit(1);
          }
          else
          {
               c = wait(&status);
               printf("PID %ld exited with status %d\n", (long)c, status);
          }
     }
}

/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */

int main(int argc, char *argv[])
{
     FILE *fp;
     fp = stdin; // default stream is stdin

     if (argc > 1) // batchmode...
     {
          printf("FILE IS: %s, argv[1] is %s\n", argv[1], argv[1]);
          fp = fopen(argv[1], "r"); // open file for read

          if (fp == NULL) // error handling...
          {
               perror("fopen()");
               exit(0);
          }
     }

     char line[1024]; /* the input line                 */
     char *arg[64];   /* the command line argument      */
     char *iArg[64];  /* the command line argument      */
     size_t count;

     printf("prompt> "); /*  display a prompt  */
     while (fgets(line, sizeof(line), fp))
     {
          count = parse(line, arg, ";"); /* parse piped commands */

          for (int i = 0; i < count; ++i)
          {
               parse(arg[i], iArg, " \t\n");
               execute(iArg); /* otherwise, execute the command */
          }
          printf("shell> "); // display prompt
     }
     return 0;
}
