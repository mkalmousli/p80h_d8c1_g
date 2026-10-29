/* SPDX-License-Identifier: MIT */
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int daemon_command(const char *command)
{
    enum { LIMIT = 4 * 1024 * 1024 };
    char *data = malloc(LIMIT + 1);
    size_t used = 0;
    int fd = daemon_open(command), result = 1;
    if (!data || fd < 0) { fputs("Daemon unavailable or invalid endpoint.\n", stderr); goto done; }
    while (used < LIMIT) {
        ssize_t n = net_read(fd, data + used, LIMIT - used);
        if (n < 0) goto done;
        if (!n) break;
        used += (size_t)n;
    }
    if (used == LIMIT) { fputs("Response exceeds 4 MiB limit.\n", stderr); goto done; }
    data[used] = 0;
    if (!strncmp(command, "EXEC ", 5)) {
        char *trailer = NULL;
        for (char *p = data; (p = strstr(p, "\n@@")); p++) trailer = p;
        if (!trailer || used == 0 || data[used - 1] != '\n') goto done;
        unsigned code;
        int consumed = 0;
        if (!strcmp(trailer, "\n@@TIMEOUT\n")) result = 124;
        else if (sscanf(trailer, "\n@@EXIT %u\n%n", &code, &consumed) == 1 &&
                 consumed > 0 && !trailer[consumed] && code <= 255) result = (int)code;
        else goto done;
        used = (size_t)(trailer - data);
    } else result = used ? 0 : 1;
    if (fwrite(data, 1, used, stdout) != used || fflush(stdout)) result = 1;
done:
    if (fd >= 0) close(fd);
    free(data);
    return result;
}
