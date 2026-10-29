/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "server.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

/* GET: offset:u64, limit:u32, path bytes. Reply: total size:u64 and raw bytes. */
int serve_get(const struct wire_message *request, struct wire_message *reply)
{
    if (request->size < 13 || request->size > 1035 ||
        memchr(request->body + 12, 0, request->size - 12)) return WIRE_INVALID;
    char path[1024];
    size_t length = request->size - 12;
    memcpy(path, request->body + 12, length); path[length] = 0;
    uint64_t offset = wire_u64(request->body), total = 0;
    uint32_t limit = wire_u32(request->body + 8);
    if (path[0] != '/' || limit > WIRE_LIMIT - 8 || offset > INT64_MAX) return WIRE_INVALID;
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    struct stat st;
    if (fd < 0) return WIRE_IO;
    int result = WIRE_IO;
    if (fstat(fd, &st)) goto done;
    if (S_ISREG(st.st_mode) && st.st_size >= 0) total = (uint64_t)st.st_size;
    else if (!S_ISBLK(st.st_mode) || ioctl(fd, BLKGETSIZE64, &total)) goto done;
    if (offset > total || total > INT64_MAX) { result = WIRE_INVALID; goto done; }
    if (limit > total - offset) limit = (uint32_t)(total - offset);
    reply->body = malloc((size_t)limit + 8);
    if (!reply->body) goto done;
    wire_put64(reply->body, total);
    size_t used = 0;
    while (used < limit) {
        ssize_t n = pread(fd, reply->body + 8 + used, limit - used, (off_t)(offset + used));
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) goto done;
        used += (size_t)n;
    }
    reply->size = limit + 8; result = WIRE_OK;
done:
    close(fd); return result;
}
