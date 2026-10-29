/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "link-client.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
/* One 64 KiB GET at a time is reliable on this USB stack; every response is ID/CRC checked. */
/* Explicit resume appends to an existing regular file; ordinary get never overwrites. */
static int download(struct link_session *s, const char *remote, const char *local, int resume)
{
    size_t length = strlen(remote);
    if (!length || length > 1023) return 1;
    unsigned char body[1035] = {0};
    memcpy(body + 12, remote, length);
    struct wire_message reply;
    if (link_request(s, WIRE_GET, body, (uint32_t)length + 12, &reply)) return 1;
    if (reply.size != 8) { free(reply.body); return 1; }
    uint64_t total = wire_u64(reply.body), offset = 0;
    free(reply.body);
    if (total > 16ULL * 1024 * 1024 * 1024) return 1;
    int flags = resume ? O_WRONLY | O_NOFOLLOW : O_WRONLY | O_CREAT | O_EXCL;
    int fd = open(local, flags, 0600);
    int result = 1;
    if (fd < 0) { perror(local); return 1; }
    struct stat st;
    if (fstat(fd, &st) || !S_ISREG(st.st_mode) || (resume && st.st_size < 0) ||
        (resume && (uint64_t)st.st_size > total)) goto done;
    if (resume) offset = (uint64_t)st.st_size;
    if (lseek(fd, (off_t)offset, SEEK_SET) < 0) goto done;
    while (offset < total) {
        uint32_t bytes = total - offset > WIRE_CHUNK ? WIRE_CHUNK : (uint32_t)(total - offset);
        wire_put64(body, offset); wire_put32(body + 8, bytes);
        if (link_request(s, WIRE_GET, body, (uint32_t)length + 12, &reply)) goto done;
        if (reply.size != bytes + 8 || wire_u64(reply.body) != total) {
            free(reply.body); goto done;
        }
        size_t written = 0;
        while (written < bytes) {
            ssize_t n = write(fd, reply.body + 8 + written, bytes - written);
            if (n < 0 && errno == EINTR) continue;
            if (n <= 0) { free(reply.body); goto done; }
            written += (size_t)n;
        }
        free(reply.body); offset += bytes;
    }
    result = fsync(fd) != 0;
done:
    if (close(fd)) result = 1;
    if (result && !resume) unlink(local);
    return result;
}

int link_download(struct link_session *s, const char *remote, const char *local)
{ return download(s, remote, local, 0); }

int link_download_resume(struct link_session *s, const char *remote, const char *local)
{ return download(s, remote, local, 1); }
