/* SPDX-License-Identifier: MIT */
#include "link-client.h"
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Correlate every response with its request before exposing the payload. */
int link_request(struct link_session *s, uint16_t op, unsigned char *data, uint32_t size,
                 struct wire_message *reply)
{
    *reply = (struct wire_message){0};
    if (s->sequence == UINT32_MAX) return 1;
    struct wire_message request = {op, 0, 0, ++s->sequence, size, data};
    if (wire_send(s->fd, &request) || wire_recv(s->fd, reply)) return 1;
    if (reply->op != op || reply->id != request.id || reply->flags != WIRE_REPLY || reply->status) {
        fprintf(stderr, "P80H/1 response rejected: opcode=%u status=%u\n", reply->op, reply->status);
        free(reply->body); reply->body = NULL; return 1;
    }
    return 0;
}

int link_open(struct link_session *s)
{
    const char *endpoint = getenv("P80H_LINK");
    s->sequence = 0; s->fd = tcp_open(endpoint ? endpoint : "172.16.42.1:4445");
    if (s->fd < 0) { fputs("P80H/1 binary endpoint unavailable.\n", stderr); return 1; }
    struct wire_message reply;
    int failed = link_request(s, WIRE_HELLO, NULL, 0, &reply);
    if (!failed) failed = reply.size != 8 || wire_u32(reply.body) != 0x3f ||
        wire_u32(reply.body + 4) != WIRE_LIMIT;
    free(reply.body);
    if (failed) { close(s->fd); s->fd = -1; }
    return failed;
}
