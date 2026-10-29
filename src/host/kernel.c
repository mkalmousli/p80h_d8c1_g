/* SPDX-License-Identifier: MIT */
#include "run.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define KERNEL_TAG "v7.3"
#define KERNEL_COMMIT "c03c1a14507dfaf8b856c2608f9ac216021fc222"
#define KERNEL_ARCHIVE "https://cdn.kernel.org/pub/linux/kernel/v7.x/linux-7.3.tar.xz"
#define KERNEL_SHA256 "87363c24f10e06321eb362765521094e94471f6bff8bce2d6b3e40f39f004953"

/* Generic ARM64 tinyconfig audit only; this is not P80h boot support. */
int main(void)
{
    if (mkdir("local", 0700) && errno != EEXIST) return 1;
    struct stat st;
    if (stat("local/linux-7.3.tar.xz", &st) && RUN("curl", "--fail", "--location",
        "--connect-timeout", "15", "--max-time", "180", "--output", "local/linux-7.3.tar.xz", KERNEL_ARCHIVE)) return 1;
    if (RUN_TO("out/kernel-archive.sha256", "sha256sum", "local/linux-7.3.tar.xz")) return 1;
    FILE *f = fopen("out/kernel-archive.sha256", "r");
    char actual[128] = {0};
    if (!f) return 1;
    int ok = fgets(actual, sizeof(actual), f) != NULL;
    fclose(f);
    if (!ok || strncmp(actual, KERNEL_SHA256, 64) || actual[64] != ' ') return 1;
    if (mkdir("local/linux-release", 0755) && errno != EEXIST) return 1;
    if (RUN("tar", "--extract", "--xz", "--no-same-owner", "--file", "local/linux-7.3.tar.xz",
        "--directory", "local/linux-release", "--strip-components=1")) return 1;
    printf("Pinned %s (%s) archive verified; generic audit only.\n", KERNEL_TAG, KERNEL_COMMIT);
    if (RUN("make", "-C", "local/linux-release", "O=../../out/linux", "ARCH=arm64",
            "CROSS_COMPILE=aarch64-linux-gnu-", "tinyconfig")) return 1;
    if (RUN("make", "-C", "local/linux-release", "O=../../out/linux", "ARCH=arm64",
            "CROSS_COMPILE=aarch64-linux-gnu-", "KBUILD_BUILD_USER=p80h", "KBUILD_BUILD_HOST=remote",
            "KBUILD_BUILD_TIMESTAMP=2026-10-18", "-j4", "Image")) return 1;
    return RUN_TO("out/kernel-SHA256SUMS", "sha256sum", "out/linux/arch/arm64/boot/Image");
}
