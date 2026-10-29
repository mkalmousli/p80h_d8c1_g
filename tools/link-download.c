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
#define WINDOW 4

static int download(struct link_session *s, const char *remote, const char *local, int resume)
{
    size_t length = strlen(remote);
    if (!length || length > 1023) return 1;
    unsigned char body[WINDOW][1035] = {{0}};
    memcpy(body[0] + 12, remote, length);
    struct wire_message reply;
    if (link_request(s, WIRE_GET, body[0], (uint32_t)length + 12, &reply)) return 1;
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
        struct wire_message sent[WINDOW]; unsigned count = 0;
        while (count < WINDOW && offset + (uint64_t)count * WIRE_CHUNK < total) {
            uint64_t at = offset + (uint64_t)count * WIRE_CHUNK;
            uint32_t want = total - at > WIRE_CHUNK ? WIRE_CHUNK : (uint32_t)(total - at);
            wire_put64(body[count], at); wire_put32(body[count] + 8, want);
            memcpy(body[count] + 12, remote, length);
            if (s->sequence == UINT32_MAX) goto done;
            sent[count] = (struct wire_message){WIRE_GET, 0, 0, ++s->sequence,
                (uint32_t)length + 12, body[count]};
            if (wire_send(s->fd, &sent[count])) goto done;
            count++;
        }
        for (unsigned i = 0; i < count; i++) {
            if (wire_recv(s->fd, &reply)) goto done;
            uint64_t at = offset + (uint64_t)i * WIRE_CHUNK;
            uint32_t bytes = total - at > WIRE_CHUNK ? WIRE_CHUNK : (uint32_t)(total - at);
            if (reply.op != WIRE_GET || reply.id != sent[i].id || reply.flags != WIRE_REPLY ||
                reply.status || reply.size != bytes + 8 || wire_u64(reply.body) != total) {
                free(reply.body); goto done;
            }
            size_t written = 0;
            while (written < bytes) {
                ssize_t n = write(fd, reply.body + 8 + written, bytes - written);
                if (n < 0 && errno == EINTR) continue;
                if (n <= 0) { free(reply.body); goto done; }
                written += (size_t)n;
            }
            free(reply.body);
        }
        offset += (uint64_t)count * WIRE_CHUNK;
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
