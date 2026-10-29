/* SPDX-License-Identifier: MIT */
#include "wire.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>

/* TCP packet boundaries are irrelevant; every header and body is read exactly. */
static int transfer(int fd, unsigned char *p, size_t n, int sending)
{
    while (n) {
        ssize_t done = sending ? send(fd, p, n, MSG_NOSIGNAL) : recv(fd, p, n, 0);
        if (done < 0 && errno == EINTR) continue;
        if (done <= 0) return 1;
        p += done; n -= (size_t)done;
    }
    return 0;
}

int wire_send(int fd, const struct wire_message *m)
{
    unsigned char header[WIRE_HEADER];
    return wire_header(header, m) || transfer(fd, header, sizeof(header), 1) ||
        transfer(fd, m->body, m->size, 1);
}

int wire_recv(int fd, struct wire_message *m)
{
    unsigned char header[WIRE_HEADER];
    *m = (struct wire_message){0};
    if (transfer(fd, header, sizeof(header), 0) || wire_decode(header, m)) return 1;
    if (m->size) {
        m->body = malloc(m->size);
        if (!m->body) return 1;
    }
    if (transfer(fd, m->body, m->size, 0) || wire_crc(m->body, m->size) != wire_u32(header + 20)) {
        free(m->body); m->body = NULL; return 1;
    }
    return 0;
}
