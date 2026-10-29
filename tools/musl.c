/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "run.h"
#include "web.h"
#include <string.h>
#include <sys/stat.h>

static char version[32], archive[256], digest[128];

static int pin(char **v, size_t n, void *context)
{
    (void)n; (void)context;
    struct { const char *key; char *dest; size_t size; } keys[] = {
        {"version", version, sizeof(version)}, {"archive", archive, sizeof(archive)},
        {"sha256", digest, sizeof(digest)}};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        if (strcmp(keys[i].key, v[0])) continue;
        if (*keys[i].dest || strlen(v[1]) >= keys[i].size) return 1;
        strcpy(keys[i].dest, v[1]); return 0;
    }
    return 1;
}

/* Cross-build a small, fully source-available static libc for device releases. */
int main(void)
{
    if (table_read("data/musl.tsv", 2, pin, NULL) || strcmp(version, "1.2.5") ||
        strcmp(archive, "https://musl.libc.org/releases/musl-1.2.5.tar.gz") ||
        strlen(digest) != 64 || strspn(digest, "0123456789abcdef") != 64) return 1;
    if (mkdir("local", 0700) && errno != EEXIST) return 1;
    struct stat st;
    if (stat("local/musl.tar.gz", &st) && RUN("curl", "--fail", "--location", "--connect-timeout",
        "15", "--max-time", "120", "--output", "local/musl.tar.gz", archive)) return 1;
    if (RUN_TO("out/musl.sha256", "sha256sum", "local/musl.tar.gz")) return 1;
    FILE *f = fopen("out/musl.sha256", "r");
    char actual[128] = {0};
    if (!f) return 1;
    int ok = fgets(actual, sizeof(actual), f) != NULL;
    fclose(f);
    if (!ok || strncmp(actual, digest, 64) || actual[64] != ' ') return 1;
    if (mkdir("local/musl-source", 0755) && errno != EEXIST) return 1;
    if (RUN("tar", "--extract", "--gzip", "--no-same-owner", "--file", "local/musl.tar.gz",
        "--directory", "local/musl-source", "--strip-components=1")) return 1;
    char root[1024], prefix[1100];
    if (!getcwd(root, sizeof(root))) return 1;
    int n = snprintf(prefix, sizeof(prefix), "--prefix=%s/out/musl", root);
    if (n < 0 || (size_t)n >= sizeof(prefix) || chdir("local/musl-source")) return 1;
    if (RUN("env", "CC=aarch64-linux-gnu-gcc", "CROSS_COMPILE=aarch64-linux-gnu-", "./configure",
        "--target=aarch64-linux-musl", "--disable-shared", prefix) || RUN("make", "-j4", "install")) return 1;
    return 0;
}
