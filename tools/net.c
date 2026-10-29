/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int net_send(int fd, const void *data, size_t size)
{
    const unsigned char *p = data;
    while (size) {
        ssize_t n = send(fd, p, size, MSG_NOSIGNAL);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return 1;
        p += n; size -= (size_t)n;
    }
    return 0;
}

ssize_t net_read(int fd, void *data, size_t size)
{
    ssize_t n;
    do { n = recv(fd, data, size, 0); } while (n < 0 && errno == EINTR);
    return n;
}

int daemon_open(const char *header)
{
    const char *endpoint = getenv("P80H_DAEMON");
    char host[64], *colon, *end;
    struct sockaddr_in addr = {.sin_family = AF_INET};
    if (!endpoint) endpoint = "172.16.42.1:4444";
    if (strlen(endpoint) >= sizeof(host) || strpbrk(header, "\r\n")) return -1;
    strcpy(host, endpoint);
    colon = strrchr(host, ':');
    if (!colon) return -1;
    *colon++ = 0;
    errno = 0;
    unsigned long port = strtoul(colon, &end, 10);
    if (errno || !*colon || *end || !port || port > 65535 ||
        inet_pton(AF_INET, host, &addr.sin_addr) != 1) return -1;
    addr.sin_port = htons((unsigned short)port);
    int fd = socket(AF_INET, SOCK_STREAM, 0), error = 0;
    if (fd < 0) return -1;
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) goto fail;
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (errno != EINPROGRESS) goto fail;
        struct pollfd p = {fd, POLLOUT, 0};
        socklen_t length = sizeof(error);
        if (poll(&p, 1, 5000) <= 0 ||
            getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &length) || error) goto fail;
    }
    if (fcntl(fd, F_SETFL, 0) < 0) goto fail;
    struct timeval timeout = {35, 0};
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) ||
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) goto fail;
    if (net_send(fd, header, strlen(header)) || net_send(fd, "\n", 1)) goto fail;
    return fd;
fail:
    close(fd);
    return -1;
}
