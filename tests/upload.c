/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/host/net.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

/* Exercise the public staged-upload contract with binary data and a real socket. */
int main(void)
{
    const unsigned char body[] = {0, 1, 255, 10};
    FILE *f = fopen("out/upload-test", "wb");
    assert(f && fwrite(body, 1, sizeof(body), f) == sizeof(body) && !fclose(f));
    assert(daemon_put("out/upload-test", "/dev/mmcblk0"));
    assert(daemon_put("out/upload-test", "/tmp/p80h-../bad"));
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_LOOPBACK)};
    socklen_t length = sizeof(addr);
    assert(listener >= 0 && !bind(listener, (struct sockaddr *)&addr, sizeof(addr)));
    assert(!getsockname(listener, (struct sockaddr *)&addr, &length) && !listen(listener, 1));
    char endpoint[64];
    snprintf(endpoint, sizeof(endpoint), "127.0.0.1:%u", ntohs(addr.sin_port));
    assert(!setenv("P80H_DAEMON", endpoint, 1));
    pid_t child = fork();
    assert(child >= 0);
    if (!child) {
        alarm(10);
        int fd = accept(listener, NULL, NULL);
        char header[128];
        size_t used = 0;
        assert(fd >= 0);
        do { assert(used < 127 && read(fd, header + used, 1) == 1); }
        while (header[used++] != '\n');
        header[used] = 0;
        assert(!strcmp(header, "PUT /tmp/p80h-upload 4 644\n"));
        unsigned char received[4];
        for (size_t i = 0; i < sizeof(received); i++)
            assert(read(fd, received + i, 1) == 1);
        assert(!memcmp(received, body, sizeof(body)));
        assert(!net_send(fd, "OK\n", 3));
        close(fd); close(listener); _exit(0);
    }
    close(listener);
    assert(!daemon_put("out/upload-test", "/tmp/p80h-upload"));
    int status;
    assert(waitpid(child, &status, 0) == child && WIFEXITED(status) && !WEXITSTATUS(status));
    assert(!unlink("out/upload-test"));
    puts("PASS: staged binary upload, acknowledgement and device-path refusal");
    return 0;
}
