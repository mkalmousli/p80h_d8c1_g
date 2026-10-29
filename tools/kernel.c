/* SPDX-License-Identifier: MIT */
#include "run.h"
#include "web.h"
#include <string.h>
#include <sys/stat.h>

static char tag[64], commit[64], archive[256], sha256[128];

/* Lock both upstream revision and release archive bytes; never use a moving ref. */
static int pin(char **v, size_t n, void *context)
{
    (void)n; (void)context;
    struct { const char *key; char *dest; size_t size; } keys[] = {
        {"tag", tag, sizeof(tag)}, {"commit", commit, sizeof(commit)},
        {"archive", archive, sizeof(archive)}, {"sha256", sha256, sizeof(sha256)}};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        if (strcmp(v[0], keys[i].key)) continue;
        if (*keys[i].dest || strlen(v[1]) >= keys[i].size) return 1;
        strcpy(keys[i].dest, v[1]); return 0;
    }
    return 1;
}

int main(void)
{
    if (table_read("data/kernel.tsv", 2, pin, NULL) || strcmp(tag, "v7.3") ||
        strlen(commit) != 40 || strspn(commit, "0123456789abcdef") != 40 ||
        strlen(sha256) != 64 || strspn(sha256, "0123456789abcdef") != 64 ||
        strcmp(archive, "https://cdn.kernel.org/pub/linux/kernel/v7.x/linux-7.3.tar.xz")) return 1;
    if (mkdir("local", 0700) && errno != EEXIST) return 1;
    struct stat st;
    if (stat("local/linux-7.3.tar.xz", &st) && RUN("curl", "--fail", "--location",
        "--connect-timeout", "15", "--max-time", "180", "--output", "local/linux-7.3.tar.xz", archive)) return 1;
    if (RUN_TO("out/kernel-archive.sha256", "sha256sum", "local/linux-7.3.tar.xz")) return 1;
    FILE *f = fopen("out/kernel-archive.sha256", "r");
    char actual[128] = {0};
    if (!f) return 1;
    int ok = fgets(actual, sizeof(actual), f) != NULL;
    fclose(f);
    if (!ok || strncmp(actual, sha256, 64) || actual[64] != ' ') return 1;
    if (mkdir("local/linux-release", 0755) && errno != EEXIST) return 1;
    if (RUN("tar", "--extract", "--xz", "--no-same-owner", "--file", "local/linux-7.3.tar.xz",
        "--directory", "local/linux-release", "--strip-components=1")) return 1;
    puts("Pinned upstream archive verified. Audit only: not a tablet image.");
    if (RUN("make", "-C", "local/linux-release", "O=../../out/linux", "ARCH=arm64",
            "CROSS_COMPILE=aarch64-linux-gnu-", "tinyconfig")) return 1;
    if (RUN("make", "-C", "local/linux-release", "O=../../out/linux", "ARCH=arm64",
            "CROSS_COMPILE=aarch64-linux-gnu-", "KBUILD_BUILD_USER=p80h", "KBUILD_BUILD_HOST=remote",
            "KBUILD_BUILD_TIMESTAMP=2026-10-18", "-j4", "Image")) return 1;
    return RUN_TO("out/kernel-SHA256SUMS", "sha256sum", "out/linux/arch/arm64/boot/Image");
}
