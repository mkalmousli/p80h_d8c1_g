/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "link-client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* PUT commits only after every chunk has been acknowledged by the server. */
int link_upload(struct link_session *s, const char *local, const char *remote)
{
    size_t length = strlen(remote), overhead = length + 18;
    if (length <= 10 || length > 255 || strncmp(remote, "/tmp/p80h-", 10) ||
        strpbrk(remote + 10, "/\r\n\t ")) return 1;
    FILE *f = fopen(local, "rb");
    if (!f) return 1;
    struct stat st;
    int result = 1;
    unsigned char *body = malloc(WIRE_LIMIT);
    if (!body || fstat(fileno(f), &st) || !S_ISREG(st.st_mode) || st.st_size < 0 ||
        (uint64_t)st.st_size > 16ULL * 1024 * 1024 * 1024) goto done;
    wire_put16(body, (uint16_t)length); memcpy(body + 2, remote, length);
    uint64_t total = (uint64_t)st.st_size, offset = 0;
    do {
        size_t want = total - offset > WIRE_CHUNK ? WIRE_CHUNK : (size_t)(total - offset);
        wire_put64(body + 2 + length, offset); wire_put64(body + 10 + length, total);
        if (fread(body + overhead, 1, want, f) != want) goto done;
        struct wire_message reply;
        if (link_request(s, WIRE_PUT, body, (uint32_t)(overhead + want), &reply)) goto done;
        int bad = reply.size != 0;
        free(reply.body);
        if (bad) goto done;
        offset += want;
    } while (offset < total);
    result = ferror(f) != 0;
done:
    free(body); fclose(f); return result;
}
