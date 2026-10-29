/* SPDX-License-Identifier: MIT */
#include "run.h"
#include "web.h"
#include <string.h>
#include <sys/stat.h>

static char tag[64], commit[64], url[256];

/* Only explicit pinned upstream input is accepted; never use a moving branch. */
static int pin(char **v, size_t n, void *context)
{
    (void)n; (void)context;
    char *dest = NULL;
    size_t size = 0;
    if (!strcmp(v[0], "tag")) { dest = tag; size = sizeof(tag); }
    if (!strcmp(v[0], "commit")) { dest = commit; size = sizeof(commit); }
    if (!strcmp(v[0], "url")) { dest = url; size = sizeof(url); }
    if (!dest || *dest || strlen(v[1]) >= size) return 1;
    strcpy(dest, v[1]); return 0;
}

int main(void)
{
    if (table_read("data/kernel.tsv", 2, pin, NULL) || !*tag ||
        strlen(commit) != 40 || strspn(commit, "0123456789abcdef") != 40 ||
        strcmp(url, "https://github.com/torvalds/linux.git")) return 1;
    if (mkdir("local", 0700) && errno != EEXIST) return 1;
    struct stat st;
    if (stat("local/linux/.git", &st) &&
        RUN("git", "clone", "--depth=1", "--branch", tag, url, "local/linux")) return 1;
    if (RUN_TO("out/kernel-revision", "git", "-C", "local/linux", "rev-parse", "HEAD")) return 1;
    FILE *f = fopen("out/kernel-revision", "r");
    char actual[128] = {0};
    if (!f) return 1;
    int ok = fgets(actual, sizeof(actual), f) != NULL;
    fclose(f); actual[strcspn(actual, "\r\n")] = 0;
    if (!ok || strcmp(actual, commit)) { fputs("Kernel pin mismatch.\n", stderr); return 1; }
    puts("Pinned upstream fetched. This is an audit build, not a tablet image.");
    if (RUN("make", "-C", "local/linux", "O=../../out/linux", "ARCH=arm64",
            "CROSS_COMPILE=aarch64-linux-gnu-", "tinyconfig")) return 1;
    if (RUN("make", "-C", "local/linux", "O=../../out/linux", "ARCH=arm64",
            "CROSS_COMPILE=aarch64-linux-gnu-", "-j4", "Image")) return 1;
    return RUN_TO("out/kernel-SHA256SUMS", "sha256sum", "out/linux/arch/arm64/boot/Image");
}
