#include "../includes/input.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../includes/common.h"

// Input Loop of the Shell
void nash_loop(void) {
  char *line;
  char **args;
  int status;
  struct utsname uname_data;  // required for username and system name
  uname(&uname_data);
  do {
    printf("%s> ", uname_data.nodename);
    line = nash_read_line();
    args = nash_split_line(line);
    status = nash_exec(args);

    free(line);
    free(args);

  } while (status);
}

// Reading line using getline()
char *nash_read_line(void) {
  char *line = NULL;
  ssize_t bufsize = 0;  // having getline allocating a buffer
  getline(&line, &bufsize, stdin);
  return line;
}

#define NASH_TOK_BUFSIZE 64
#define NASH_TOK_DELIM " \t\n\r"  // delim chars for tokenizing input

/*
  nash_split_line works as follows
  We use strtok() to tokenize input and store the pointer to
  first token into token. While token is not null, we store
  token into tokens[position]. If position exceeds the buffer
  size then we dynamically expand the buffer. After getting the
  first token we use strtok() this time with NULL to get the next
  token.
 */

char **nash_split_line(char *line) {
  int bufsize = NASH_TOK_BUFSIZE;
  int position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "nash: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, NASH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += NASH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens) {
        fprintf(stderr, "nash: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, NASH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
