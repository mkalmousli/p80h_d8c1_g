/* SPDX-License-Identifier: MIT */
#ifndef P80H_RUN_H
#define P80H_RUN_H
#include <errno.h>
#include <fcntl.h>
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

/* Capture subprocess output without a shell, preserving the caller's streams. */
static inline int run_to(const char *path, char *const argv[])
{
    pid_t child = fork();
    if (child < 0) return 1;
    if (!child) {
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0) _exit(1);
        close(fd); execvp(argv[0], argv); _exit(127);
    }
    int status;
    while (waitpid(child, &status, 0) < 0) if (errno != EINTR) return 1;
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
#define RUN_TO(path, ...) run_to(path, (char *const[]){__VA_ARGS__, NULL})
#endif
