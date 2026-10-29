/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "net.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int daemon_put(const char *local, const char *remote)
{
    /* Staging only. Never hand device nodes to the old daemon's PUT operation. */
    if (strncmp(remote, "/tmp/p80h-", 10) || strpbrk(remote + 10, "/ \t\r\n")) {
        fputs("Uploads are restricted to /tmp/p80h-NAME staging files.\n", stderr);
        return 1;
    }
    FILE *input = fopen(local, "rb");
    if (!input) { perror(local); return 1; }
    struct stat st;
    int result = 1, fd = -1;
    if (fstat(fileno(input), &st) || !S_ISREG(st.st_mode) || st.st_size < 0 ||
        st.st_size > 256LL * 1024 * 1024) goto done;
    char header[1024], buffer[65536];
    int n = snprintf(header, sizeof(header), "PUT %s %" PRIu64 " 644", remote,
                     (uint64_t)st.st_size);
    if (n < 0 || (size_t)n >= sizeof(header)) goto done;
    fd = daemon_open(header);
    if (fd < 0) goto done;
    uint64_t remaining = (uint64_t)st.st_size;
    while (remaining) {
        size_t want = remaining < sizeof(buffer) ? (size_t)remaining : sizeof(buffer);
        if (fread(buffer, 1, want, input) != want || net_send(fd, buffer, want)) goto done;
        remaining -= want;
    }
    if (ferror(input)) goto done;
    size_t used = 0;
    while (used < 4) {
        ssize_t got = net_read(fd, buffer + used, 4 - used);
        if (got < 0) goto done;
        if (!got) break;
        used += (size_t)got;
    }
    result = used != 3 || memcmp(buffer, "OK\n", 3);
done:
    if (fd >= 0) close(fd);
    fclose(input);
    return result;
}
