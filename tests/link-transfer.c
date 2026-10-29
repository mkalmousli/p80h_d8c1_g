/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/link/server.h"
#include "tools/link-client.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    char target[128];
    snprintf(target, sizeof(target), "/tmp/p80h-transfer-%ld", (long)getpid());
    FILE *f = fopen("out/binary-source", "wb");
    assert(f);
    for (unsigned i = 0; i < 2 * WIRE_LIMIT + 731; i++) assert(fputc((int)(i % 251), f) != EOF);
    assert(!fclose(f));
    int pair[2]; assert(!socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
    pid_t child = fork(); assert(child >= 0);
    if (!child) { close(pair[0]); alarm(20); serve_connection(pair[1]); close(pair[1]); _exit(0); }
    close(pair[1]);
    struct link_session session = {pair[0], 0};
    struct wire_message reply;
    assert(!link_request(&session, WIRE_HELLO, NULL, 0, &reply)); free(reply.body);
    assert(!link_upload(&session, "out/binary-source", target));
    assert(!link_download(&session, target, "out/binary-result"));
    assert(link_download(&session, target, "out/binary-result"));
    FILE *a = fopen("out/binary-source", "rb"), *b = fopen("out/binary-result", "rb");
    assert(a && b);
    int c;
    while ((c = fgetc(a)) != EOF) assert(c == fgetc(b));
    assert(fgetc(b) == EOF && !ferror(a) && !ferror(b)); fclose(a); fclose(b);
    assert(!unlink("out/binary-result"));
    a = fopen("out/binary-source", "rb"); b = fopen("out/binary-result", "wb"); assert(a && b);
    for (unsigned i = 0; i < 12345; i++) assert(fputc(fgetc(a), b) != EOF);
    fclose(a); fclose(b);
    assert(!link_download_resume(&session, target, "out/binary-result"));
    a = fopen("out/binary-source", "rb"); b = fopen("out/binary-result", "rb"); assert(a && b);
    while ((c = fgetc(a)) != EOF) assert(c == fgetc(b));
    assert(fgetc(b) == EOF); fclose(a); fclose(b);
    assert(!unlink(target));
    /* A disconnect during an upload must not make a partial destination visible. */
    size_t length = strlen(target);
    unsigned char partial[256] = {0};
    wire_put16(partial, (uint16_t)length); memcpy(partial + 2, target, length);
    wire_put64(partial + 10 + length, 10); partial[18 + length] = 1;
    assert(!link_request(&session, WIRE_PUT, partial, (uint32_t)length + 19, &reply));
    free(reply.body); assert(access(target, F_OK)); close(pair[0]);
    int status;
    assert(waitpid(child, &status, 0) == child && WIFEXITED(status) && !WEXITSTATUS(status));
    char temporary[160];
    snprintf(temporary, sizeof(temporary), "%s.part-%ld", target, (long)child);
    assert(access(temporary, F_OK));
    assert(!unlink("out/binary-source") && !unlink("out/binary-result"));
    puts("PASS: binary multi-chunk round trip, no overwrite and interrupted-upload cleanup");
    return 0;
}
