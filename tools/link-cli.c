/* SPDX-License-Identifier: MIT */
#include "link-client.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Human-facing output is text; every tablet message is a binary P80H/1 frame. */
int link_cli(int argc, char **argv)
{
    uint16_t op;
    unsigned char *body = NULL;
    uint32_t length = 0;
    if (argc == 2 && !strcmp(argv[1], "ping")) op = WIRE_PING;
    else if (argc == 2 && !strcmp(argv[1], "status")) op = WIRE_STATUS;
    else if (argc == 3 && !strcmp(argv[1], "sh")) {
        op = WIRE_EXEC; body = (unsigned char *)argv[2];
        if (strlen(argv[2]) > 4096) return 2;
        length = (uint32_t)strlen(argv[2]);
    } else if (argc == 4 && (!strcmp(argv[1], "get") || !strcmp(argv[1], "put"))) op = WIRE_GET;
    else {
        puts("p80h P80H/1: ping | status | sh 'COMMAND' | get REMOTE LOCAL | put LOCAL /tmp/p80h-NAME\n"
             "Local: usb | inspect IMAGE | gpt SNAPSHOT | rom-info\nTransitional text endpoint: p80h legacy COMMAND");
        return argc == 1 ? 0 : 2;
    }
    struct link_session session;
    if (link_open(&session)) return 1;
    int result = 1;
    if (argc == 4) {
        result = !strcmp(argv[1], "get") ? link_download(&session, argv[2], argv[3]) :
            link_upload(&session, argv[2], argv[3]);
        close(session.fd); return result;
    }
    struct wire_message reply;
    if (link_request(&session, op, body, length, &reply)) { close(session.fd); return 1; }
    if (op == WIRE_PING && !reply.size) { puts("P80H/1.0 · binary link ready"); result = 0; }
    if (op == WIRE_STATUS && reply.size == 32) {
        printf("P80H/1.0 · uptime=%" PRIu64 " ms · available=%" PRIu64 " bytes\n",
               wire_u64(reply.body), wire_u64(reply.body + 8));
        printf("battery: voltage=%" PRIu32 " uV · capacity=%" PRIu32 "%% · availability=0x%x\n",
               wire_u32(reply.body + 16), wire_u32(reply.body + 20), wire_u32(reply.body + 24));
        result = 0;
    }
    if (op == WIRE_EXEC && reply.size >= 4) {
        uint32_t code = wire_u32(reply.body);
        result = code <= 255 ? (int)code : 1;
        if (fwrite(reply.body + 4, 1, reply.size - 4, stdout) != reply.size - 4 || fflush(stdout)) result = 1;
    }
    free(reply.body); close(session.fd); return result;
}
