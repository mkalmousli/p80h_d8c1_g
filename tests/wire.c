/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/link/server.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

static void request(int fd, uint16_t op, uint32_t id, const char *text, uint16_t expected)
{
    struct wire_message m = {op, 0, 0, id, text ? (uint32_t)strlen(text) : 0, (unsigned char *)text};
    assert(!wire_send(fd, &m));
    struct wire_message reply;
    assert(!wire_recv(fd, &reply));
    assert(reply.flags == WIRE_REPLY && reply.id == id && reply.op == op && reply.status == expected);
    if (op == WIRE_EXEC && expected == WIRE_OK)
        assert(reply.size == 7 && wire_u32(reply.body) == 7 && !memcmp(reply.body + 4, "abc", 3));
    if (op == WIRE_STATUS && expected == WIRE_OK) assert(reply.size == 32);
    if (op == WIRE_HELLO && expected == WIRE_OK)
        assert(reply.size == 8 && wire_u32(reply.body + 4) == WIRE_LIMIT);
    free(reply.body);
}

int main(void)
{
    unsigned char header[24];
    struct wire_message m = {WIRE_PING, 0, 0, 42, 0, NULL}, decoded;
    assert(!wire_header(header, &m) && !wire_decode(header, &decoded));
    assert(decoded.id == 42 && decoded.op == WIRE_PING);
    header[4] = 2; assert(wire_decode(header, &decoded)); header[4] = 1;
    wire_put32(header + 16, WIRE_LIMIT + 1); assert(wire_decode(header, &decoded));
    assert(wire_crc((const unsigned char *)"123456789", 9) == 0xcbf43926);
    int pair[2]; assert(!socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
    pid_t child = fork(); assert(child >= 0);
    if (!child) { close(pair[0]); alarm(15); serve_connection(pair[1]); close(pair[1]); _exit(0); }
    close(pair[1]);
    request(pair[0], WIRE_STATUS, 1, NULL, WIRE_STATE);
    request(pair[0], WIRE_HELLO, 2, NULL, WIRE_OK);
    request(pair[0], WIRE_PING, 3, NULL, WIRE_OK);
    request(pair[0], WIRE_STATUS, 4, NULL, WIRE_OK);
    request(pair[0], WIRE_EXEC, 5, "printf abc; exit 7", WIRE_OK);
    request(pair[0], 65535, 6, NULL, WIRE_UNSUPPORTED);
    request(pair[0], WIRE_PING, 6, NULL, WIRE_INVALID);
    close(pair[0]);
    int status;
    assert(waitpid(child, &status, 0) == child && WIFEXITED(status) && !WEXITSTATUS(status));
    assert(!socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
    unsigned char body[] = {1, 2, 3};
    m = (struct wire_message){WIRE_PUT, 0, 0, 1, 3, body};
    assert(!wire_header(header, &m)); body[0] ^= 1;
    assert(write(pair[0], header, 24) == 24 && write(pair[0], body, 3) == 3);
    assert(wire_recv(pair[1], &decoded)); close(pair[0]); close(pair[1]);
    puts("PASS: P80H/1 negotiation, correlation, binary status, exit codes, versions and CRC failures");
    return 0;
}
