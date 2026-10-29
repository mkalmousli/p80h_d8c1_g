/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "server.h"
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* EXEC is an explicit root shell operation, bounded to 30 s and 1 MiB output. */
int serve_exec(const struct wire_message *request, struct wire_message *reply)
{
    if (!request->size || request->size > 4096 || memchr(request->body, 0, request->size))
        return WIRE_INVALID;
    char command[4097];
    memcpy(command, request->body, request->size); command[request->size] = 0;
    reply->body = malloc(WIRE_LIMIT);
    if (!reply->body) return WIRE_IO;
    int pipes[2];
    if (pipe(pipes)) return WIRE_IO;
    pid_t child = fork();
    if (child < 0) { close(pipes[0]); close(pipes[1]); return WIRE_IO; }
    if (!child) {
        close(pipes[0]);
        if (setsid() < 0 || dup2(pipes[1], 1) < 0 || dup2(pipes[1], 2) < 0) _exit(127);
        close(pipes[1]);
        execl("/bin/sh", "sh", "-c", command, (char *)NULL); _exit(127);
    }
    close(pipes[1]);
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    size_t used = 4;
    int result = WIRE_OK, status = 0;
    for (;;) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        if (now.tv_sec - start.tv_sec >= 30) { result = WIRE_TIMEOUT; break; }
        struct pollfd p = {pipes[0], POLLIN, 0};
        int ready = poll(&p, 1, 100);
        if (ready < 0 && errno == EINTR) continue;
        if (ready < 0) { result = WIRE_IO; break; }
        if (!ready) continue;
        if (used == WIRE_LIMIT) { result = WIRE_TOO_LARGE; break; }
        ssize_t n = read(pipes[0], reply->body + used, WIRE_LIMIT - used);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) { result = WIRE_IO; break; }
        if (!n) break;
        used += (size_t)n;
    }
    close(pipes[0]);
    if (result != WIRE_OK) kill(-child, SIGKILL);
    pid_t waited;
    do { waited = waitpid(child, &status, 0); } while (waited < 0 && errno == EINTR);
    if (waited != child) return WIRE_IO;
    if (result != WIRE_OK) return result;
    wire_put32(reply->body, WIFEXITED(status) ? (uint32_t)WEXITSTATUS(status) :
               WIFSIGNALED(status) ? (uint32_t)(128 + WTERMSIG(status)) : 255);
    reply->size = (uint32_t)used;
    return WIRE_OK;
}
