/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "image.h"
#include "net.h"
#include <inttypes.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
int usb_list(void);

static int range(const char *remote, const char *local, const char *a, const char *b)
{
    const char *texts[] = {a, b};
    uint64_t values[2];
    for (unsigned i = 0; i < 2; i++) {
        if (!*texts[i] || strspn(texts[i], "0123456789") != strlen(texts[i])) return 2;
        char *end;
        errno = 0;
        values[i] = strtoull(texts[i], &end, 10);
        if (errno || *end) return 2;
    }
    return daemon_get_range(remote, local, values[0], values[1]);
}

static int inspect(const char *path)
{
    unsigned char header[1632];
    struct boot_image image;
    struct stat st;
    FILE *f = fopen(path, "rb");
    if (!f) { perror(path); return 1; }
    bool ok = !fstat(fileno(f), &st) && S_ISREG(st.st_mode) && st.st_size >= 0;
    size_t n = fread(header, 1, sizeof(header), f);
    ok = ok && !ferror(f) && image_parse(header, n, (uint64_t)st.st_size, &image);
    fclose(f);
    if (!ok) { fputs("Unsupported or malformed legacy boot image.\n", stderr); return 1; }
    printf("Android v0: kernel=%" PRIu32 " ramdisk=%" PRIu32
           " second=%" PRIu32 " page=%" PRIu32 " end=%" PRIu64 "\n",
           image.kernel, image.ramdisk, image.second, image.page, image.end);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "usb")) return usb_list();
    if (argc == 2 && !strcmp(argv[1], "status")) return daemon_command("STATUS");
    if (argc == 2 && !strcmp(argv[1], "ping")) return daemon_command("PING");
    if (argc == 3 && !strcmp(argv[1], "sh")) {
        char header[4096];
        int n = snprintf(header, sizeof(header), "EXEC 30 %s", argv[2]);
        return n < 0 || (size_t)n >= sizeof(header) ? 2 : daemon_command(header);
    }
    if (argc == 4 && !strcmp(argv[1], "get")) return daemon_get(argv[2], argv[3]);
    if (argc == 6 && !strcmp(argv[1], "get")) return range(argv[2], argv[3], argv[4], argv[5]);
    if (argc == 4 && !strcmp(argv[1], "put")) return daemon_put(argv[2], argv[3]);
    if (argc == 3 && !strcmp(argv[1], "inspect")) return inspect(argv[2]);
    puts("p80h: usb | inspect IMAGE | ping | status | sh 'COMMAND'\n"
         "      get REMOTE LOCAL [OFFSET LENGTH] | put LOCAL /tmp/p80h-NAME");
    return argc == 1 ? 0 : 2;
}
