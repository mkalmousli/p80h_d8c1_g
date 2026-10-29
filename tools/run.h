/* SPDX-License-Identifier: MIT */
#ifndef P80H_RUN_H
#define P80H_RUN_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int run(char *const argv[])
{
    pid_t child = fork();
    int status;
    if (child < 0) { perror("fork"); return 1; }
    if (child == 0) {
        execvp(argv[0], argv);
        perror(argv[0]);
        _exit(127);
    }
    while (waitpid(child, &status, 0) < 0) {
        if (errno != EINTR) { perror("waitpid"); return 1; }
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
#define RUN(...) run((char *const[]){__VA_ARGS__, NULL})
#endif
