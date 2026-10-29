/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "run.h"
#include <sys/stat.h>
#include <string.h>

#define MUSL_ARCHIVE "https://musl.libc.org/releases/musl-1.2.5.tar.gz"
#define MUSL_SHA256 "a9a118bbe84d8764da0ea0d28b3ab3fae8477fc7e4085d90102b8596fc7c75e4"

/* Build the ARM64 device server against pinned, source-available musl. */
int main(void)
{
    if (mkdir("local", 0700) && errno != EEXIST) return 1;
    struct stat st;
    if (stat("local/musl.tar.gz", &st) && RUN("curl", "--fail", "--location", "--connect-timeout",
        "15", "--max-time", "120", "--output", "local/musl.tar.gz", MUSL_ARCHIVE)) return 1;
    if (RUN_TO("out/musl.sha256", "sha256sum", "local/musl.tar.gz")) return 1;
    FILE *f = fopen("out/musl.sha256", "r");
    char actual[128] = {0};
    if (!f) return 1;
    int ok = fgets(actual, sizeof(actual), f) != NULL;
    fclose(f);
    if (!ok || strncmp(actual, MUSL_SHA256, 64) || actual[64] != ' ') return 1;
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
