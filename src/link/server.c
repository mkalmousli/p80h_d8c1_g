/* SPDX-License-Identifier: MIT */
#include "server.h"
#include <stdlib.h>

/* HELLO is mandatory; the cap permits full-device chunked reads. */
void serve_connection(int fd)
{
    int negotiated = 0;
    uint32_t previous = 0;
    for (unsigned i = 0; i < WIRE_SESSION_REQUESTS; i++) {
        struct wire_message request, response = {0};
        if (wire_recv(fd, &request)) break;
        response.op = request.op; response.id = request.id; response.flags = WIRE_REPLY;
        if (request.flags || request.status || request.id <= previous) response.status = WIRE_INVALID;
        else if (!negotiated && request.op != WIRE_HELLO) response.status = WIRE_STATE;
        else switch (request.op) {
        case WIRE_HELLO:
            if (request.size || negotiated) { response.status = WIRE_INVALID; break; }
            response.body = calloc(1, 8);
            if (!response.body) { response.status = WIRE_IO; break; }
            wire_put32(response.body, 0x3f); wire_put32(response.body + 4, WIRE_LIMIT);
            response.size = 8; negotiated = 1; break;
        case WIRE_PING:
            if (request.size) response.status = WIRE_INVALID;
            break;
        case WIRE_STATUS:
            response.status = request.size ? WIRE_INVALID : (uint16_t)serve_status(&response);
            break;
        case WIRE_EXEC: response.status = (uint16_t)serve_exec(&request, &response); break;
        case WIRE_GET: response.status = (uint16_t)serve_get(&request, &response); break;
        case WIRE_PUT: response.status = (uint16_t)serve_put(&request, &response); break;
        default: response.status = WIRE_UNSUPPORTED;
        }
        if (request.id > previous) previous = request.id;
        int failed = wire_send(fd, &response);
        free(request.body); free(response.body);
        if (failed) break;
    }
    serve_upload_close();
}
