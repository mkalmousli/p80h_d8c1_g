/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "server.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

/* Bind only the established USB gadget address; never expose root to Wi-Fi. */
int main(void)
{
    int listener = socket(AF_INET, SOCK_STREAM, 0), reuse = 1;
    struct sockaddr_in address = {.sin_family = AF_INET, .sin_port = htons(4445)};
    if (listener < 0 || inet_pton(AF_INET, "172.16.42.1", &address.sin_addr) != 1) return 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) ||
        bind(listener, (struct sockaddr *)&address, sizeof(address)) || listen(listener, 4)) {
        perror("p80h-link USB bind"); close(listener); return 1;
    }
    unsigned children = 0;
    for (;;) {
        while (waitpid(-1, NULL, WNOHANG) > 0) if (children) children--;
        int fd = accept(listener, NULL, NULL);
        if (fd < 0) { if (errno == EINTR) continue; break; }
        if (children >= 4) { close(fd); continue; }
        pid_t child = fork();
        if (!child) {
            close(listener);
            struct timeval timeout = {35, 0};
            if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) ||
                setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) _exit(1);
            if (fcntl(fd, F_SETFD, FD_CLOEXEC)) _exit(1);
            serve_connection(fd); close(fd); _exit(0);
        }
        if (child > 0) children++;
        close(fd);
    }
    close(listener);
    return 1;
}
