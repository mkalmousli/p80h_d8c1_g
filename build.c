/* SPDX-License-Identifier: MIT */
#include "src/host/targets.h"
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    const char *target = argc == 2 ? argv[1] : "help";
    if (!strcmp(target, "help")) {
        puts("Usage: ./build {package|host|device|release|kernel-audit}"); return 0;
    }
    if (strcmp(target, "package") && strcmp(target, "host") &&
        strcmp(target, "device") && strcmp(target, "release") && strcmp(target, "kernel-audit")) return 2;
    if (mkdir("out", 0755) && errno != EEXIST) return 1;
    if (!strcmp(target, "kernel-audit"))
        return BUILD("out/kernel-lab", "src/host/kernel.c") || RUN("out/kernel-lab");
    if (build_host() || RUN("out/policy")) return 1;
    if (!strcmp(target, "host")) return 0;
    if (!strcmp(target, "device") || !strcmp(target, "release")) {
        if (BUILD("out/musl-lab", "src/host/musl.c") || RUN("out/musl-lab") ||
            DEVICE("out/p80h-link-aarch64", "src/link/listen.c", SERVER_SOURCES, WIRE_SOURCES)) return 1;
    }
    if (!strcmp(target, "release")) return RUN("out/package", "--device");
    if (!strcmp(target, "package")) return RUN("out/package");
    return 0;
}
