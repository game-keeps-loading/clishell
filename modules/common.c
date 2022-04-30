#include "../includes/common.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

// To launch executables by fork() and execvp()
int nash_launch(char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0) {
    // child process
    if (execvp(args[0], args) == -1) {
      perror("nash");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // error forking
    perror("nash");
  } else {
    /*
      fork() has executed successfully.
      parent process has to wait for the child process to finish.
      waitpid() is used to wait for process' pid to change.
      Appropriate macros are used to wait until either the processes
      are exited or killed.
    */
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

// defining builtins
char *builtins[] = {"cd", "help", "pwd", "exit"};

// declaring builtin_funcs as array of pointer to function(char **) returning
// int
int (*builtin_funcs[])(char **) = {&cd, &help, &pwd, &nash_exit};

int num_builtins() { return sizeof(builtins) / sizeof(char *); }
//cd
int cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "nash: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("nash");
    }
  }
  return 1;
}
//help
int help(char **args) {
  printf("Girish and Manuj's Shell\n");
  printf("Builtin commands are cd,help,pwd and exit\n");
  return 1;
}
//to print current working directory
int pwd(char **args) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("cwd: %s\n", cwd);
  } else {
    perror("nash: Error printing current directory\n");
    return -1;
  }
  return 1;
}

int nash_exit(char **args) { return 0; }
//
int nash_exec(char **args) {
  int i;
  if (args[0] == NULL) {
    return 1;
  }
//to execte appropriate command
  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtins[i]) == 0) {
      return (*builtin_funcs[i])(args);
    }
  }
  return nash_launch(args);
}
