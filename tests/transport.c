/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/host/net.h"
#include <arpa/inet.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

static pid_t server(const char *expected, const char *reply)
{
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_LOOPBACK)};
    socklen_t length = sizeof(addr);
    assert(listener >= 0 && !bind(listener, (struct sockaddr *)&addr, sizeof(addr)));
    assert(!getsockname(listener, (struct sockaddr *)&addr, &length));
    assert(!listen(listener, 1));
    char endpoint[64];
    snprintf(endpoint, sizeof(endpoint), "127.0.0.1:%u", ntohs(addr.sin_port));
    assert(!setenv("P80H_DAEMON", endpoint, 1));
    pid_t pid = fork();
    assert(pid >= 0);
    if (!pid) {
        alarm(10);
        int fd = accept(listener, NULL, NULL);
        assert(fd >= 0);
        char header[256];
        size_t used = 0;
        do { assert(used < sizeof(header) - 1); assert(read(fd, header + used, 1) == 1); }
        while (header[used++] != '\n');
        header[used] = 0;
        assert(!strcmp(header, expected));
        /* Deliberate byte-sized writes exercise framing independently of reads. */
        for (size_t i = 0; reply[i]; i++)
            if (net_send(fd, reply + i, 1)) break;
        close(fd); close(listener); _exit(0);
    }
    close(listener);
    return pid;
}

static void joined(pid_t pid)
{
    int status;
    assert(waitpid(pid, &status, 0) == pid);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

int main(void)
{
    pid_t pid = server("EXEC 30 false\n", "output\n@@EXIT 7\n");
    assert(daemon_command("EXEC 30 false") == 7); joined(pid);
    pid = server("EXEC 30 sleep\n", "\n@@TIMEOUT\n");
    assert(daemon_command("EXEC 30 sleep") == 124); joined(pid);
    pid = server("EXEC 30 x\n", "truncated");
    assert(daemon_command("EXEC 30 x") == 1); joined(pid);
    pid = server("GET /tmp/input 0\n", "5\nhello");
    assert(!daemon_get("/tmp/input", "out/download-test")); joined(pid);
    FILE *f = fopen("out/download-test", "r");
    char b[6] = {0};
    assert(f && fread(b, 1, 5, f) == 5 && !strcmp(b, "hello")); fclose(f);
    pid = server("GET /tmp/input 0\n", "5\nhello");
    assert(daemon_get("/tmp/input", "out/download-test") == 1); joined(pid);
    assert(!unlink("out/download-test"));
    pid = server("GET /tmp/input 0\n", "5\nhi");
    assert(daemon_get("/tmp/input", "out/download-test") == 1); joined(pid);
    assert(access("out/download-test", F_OK));
    assert(!setenv("P80H_DAEMON", "127.0.0.1:99999", 1));
    assert(daemon_open("PING") < 0);
    assert(daemon_open("PING\nSTATUS") < 0);
    puts("\nPASS: fragmented daemon framing, exit status, short transfer and overwrite refusal");
    return 0;
}
