/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "net.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Frame header first, then stream exact bytes into a new owner-only local file. */
static int get(const char *remote, const char *local, uint64_t offset, uint64_t size)
{
    char header[1024], sizebuf[32], buffer[65536];
    int n = size == UINT64_MAX ? snprintf(header, sizeof(header), "GET %s 0", remote) :
        snprintf(header, sizeof(header), "GET %s %" PRIu64 " %" PRIu64, remote, offset, size);
    if (strpbrk(remote, " \t\r\n") || n < 0 || (size_t)n >= sizeof(header)) return 1;
    int fd = daemon_open(header), output = -1, result = 1;
    if (fd < 0) return 1;
    size_t used = 0;
    do {
        if (used == sizeof(sizebuf) - 1 || net_read(fd, sizebuf + used, 1) != 1) goto done;
    } while (sizebuf[used++] != '\n');
    sizebuf[used - 1] = 0;
    for (size_t i = 0; i + 1 < used; i++)
        if (sizebuf[i] < '0' || sizebuf[i] > '9') goto done;
    errno = 0;
    char *end;
    unsigned long long remaining = strtoull(sizebuf, &end, 10);
    if (errno || end == sizebuf || *end || remaining > 256ULL * 1024 * 1024) goto done;
    if (size != UINT64_MAX && remaining != size) goto done;
    output = open(local, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (output < 0) { perror(local); goto done; }
    while (remaining) {
        size_t want = remaining < sizeof(buffer) ? (size_t)remaining : sizeof(buffer);
        ssize_t got = net_read(fd, buffer, want);
        if (got <= 0) goto done;
        size_t offset = 0;
        while (offset < (size_t)got) {
            ssize_t written = write(output, buffer + offset, (size_t)got - offset);
            if (written < 0 && errno == EINTR) continue;
            if (written <= 0) goto done;
            offset += (size_t)written;
        }
        remaining -= (size_t)got;
    }
    if (net_read(fd, buffer, 1) != 0 || fsync(output)) goto done;
    result = 0;
done:
    close(fd);
    if (output >= 0) {
        if (close(output)) result = 1;
        if (result) unlink(local);
    }
    return result;
}

int daemon_get(const char *remote, const char *local)
{
    return get(remote, local, 0, UINT64_MAX);
}

int daemon_get_range(const char *remote, const char *local, uint64_t offset, uint64_t size)
{
    if (offset > INT64_MAX || size > 256ULL * 1024 * 1024 || size > INT64_MAX - offset)
        return 1;
    return get(remote, local, offset, size);
}
