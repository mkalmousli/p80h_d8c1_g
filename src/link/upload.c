/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "server.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* One upload per connection. Its staging file is invisible until final commit. */
static int output = -1;
static char destination[256], temporary[320];
static uint64_t expected_total, position;

void serve_upload_close(void)
{
    if (output >= 0) close(output);
    if (*temporary) unlink(temporary);
    output = -1; *temporary = 0; *destination = 0; position = expected_total = 0;
}

/* PUT: path length:u16, path, offset:u64, total:u64, chunk. Max total 16 GiB. */
int serve_put(const struct wire_message *request, struct wire_message *reply)
{
    (void)reply;
    if (request->size < 19) return WIRE_INVALID;
    unsigned length = wire_u16(request->body);
    if (length <= 10 || length > 255 || length + 18 > request->size ||
        memchr(request->body + 2, 0, length)) return WIRE_INVALID;
    char path[256];
    memcpy(path, request->body + 2, length); path[length] = 0;
    if (strncmp(path, "/tmp/p80h-", 10) || strpbrk(path + 10, "/\r\n\t ")) return WIRE_INVALID;
    uint64_t offset = wire_u64(request->body + 2 + length);
    uint64_t total = wire_u64(request->body + 10 + length);
    size_t bytes = request->size - length - 18;
    if (total > 16ULL * 1024 * 1024 * 1024 || offset > total || bytes > total - offset ||
        (!bytes && total)) return WIRE_INVALID;
    if (output < 0) {
        if (offset) return WIRE_STATE;
        strcpy(destination, path); expected_total = total;
        int n = snprintf(temporary, sizeof(temporary), "%s.part-%ld", path, (long)getpid());
        if (n < 0 || (size_t)n >= sizeof(temporary)) return WIRE_INVALID;
        output = open(temporary, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
        if (output < 0) { *temporary = 0; return WIRE_IO; }
    }
    if (strcmp(path, destination) || offset != position || total != expected_total) return WIRE_STATE;
    size_t used = 0;
    while (used < bytes) {
        ssize_t n = write(output, request->body + length + 18 + used, bytes - used);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { serve_upload_close(); return WIRE_IO; }
        used += (size_t)n;
    }
    position += bytes;
    if (position < total) return WIRE_OK;
    if (fsync(output) || fchmod(output, 0644)) { serve_upload_close(); return WIRE_IO; }
    int result = close(output); output = -1;
    if (result || rename(temporary, destination)) { serve_upload_close(); return WIRE_IO; }
    *temporary = 0; *destination = 0; position = expected_total = 0;
    return WIRE_OK;
}
