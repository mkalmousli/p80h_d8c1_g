/* SPDX-License-Identifier: MIT */
#include "tools/targets.h"
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    const char *codespaces = getenv("CODESPACES"), *actions = getenv("GITHUB_ACTIONS");
    const char *target = argc == 2 ? argv[1] : "help";
    if (!strcmp(target, "help")) {
        puts("Remote only: ./build {test|site|package|tools|device|release|kernel-audit}"); return 0;
    }
    /* The host PC is for USB and transfers; never compile there. */
    if ((!codespaces || strcmp(codespaces, "true")) && (!actions || strcmp(actions, "true"))) {
        fputs("Builds run in Codespaces or Actions only.\n", stderr); return 1;
    }
    if (strcmp(target, "test") && strcmp(target, "site") && strcmp(target, "package") &&
        strcmp(target, "tools") && strcmp(target, "device") && strcmp(target, "release") && strcmp(target, "kernel-audit")) return 2;
    if (mkdir("out", 0755) && errno != EEXIST) return 1;
    if (!strcmp(target, "kernel-audit"))
        return BUILD("out/kernel-lab", "tools/kernel.c", "tools/table.c") || RUN("out/kernel-lab");
    if (build_tools() || RUN("out/policy")) return 1;
    if (!strcmp(target, "tools")) return 0;
    if (build_tests() || RUN("out/sitegen")) return 1;
    if (BUILD("out/linkcheck", "tools/links.c", "tools/html.c", "tools/table.c") || RUN("out/linkcheck")) return 1;
    if (!strcmp(target, "device") || !strcmp(target, "release")) {
        if (BUILD("out/musl-lab", "tools/musl.c", "tools/table.c") || RUN("out/musl-lab") ||
            DEVICE("out/p80h-link-aarch64", "src/link/listen.c", SERVER_SOURCES, WIRE_SOURCES)) return 1;
    }
    if (!strcmp(target, "release")) return RUN("out/package", "--device");
    if (!strcmp(target, "package")) return RUN("out/package");
    return 0;
}
