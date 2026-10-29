/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "link-client.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Chunked GET supports block devices; stable size does not imply a live snapshot. */
int link_download(struct link_session *s, const char *remote, const char *local)
{
    size_t length = strlen(remote);
    if (!length || length > 1023) return 1;
    unsigned char request[1035];
    memset(request, 0, 12); memcpy(request + 12, remote, length);
    struct wire_message reply;
    if (link_request(s, WIRE_GET, request, (uint32_t)length + 12, &reply)) return 1;
    if (reply.size != 8) { free(reply.body); return 1; }
    uint64_t total = wire_u64(reply.body), offset = 0;
    free(reply.body);
    if (total > 16ULL * 1024 * 1024 * 1024) return 1;
    int fd = open(local, O_WRONLY | O_CREAT | O_EXCL, 0600), result = 1;
    if (fd < 0) { perror(local); return 1; }
    while (offset < total) {
        uint32_t want = total - offset > WIRE_CHUNK ? WIRE_CHUNK : (uint32_t)(total - offset);
        wire_put64(request, offset); wire_put32(request + 8, want);
        if (link_request(s, WIRE_GET, request, (uint32_t)length + 12, &reply)) goto done;
        if (reply.size != want + 8 || wire_u64(reply.body) != total) { free(reply.body); goto done; }
        size_t written = 0;
        while (written < want) {
            ssize_t n = write(fd, reply.body + 8 + written, want - written);
            if (n < 0 && errno == EINTR) continue;
            if (n <= 0) { free(reply.body); goto done; }
            written += (size_t)n;
        }
        free(reply.body); offset += want;
    }
    result = fsync(fd) != 0;
done:
    if (close(fd)) result = 1;
    if (result) unlink(local);
    return result;
}
