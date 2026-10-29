/* SPDX-License-Identifier: MIT */
#include "tools/run.h"
#include <string.h>
#include <sys/stat.h>

static int compile(const char *output, const char *a, const char *b,
                   const char *c)
{
    /* Bootstrap small tools without a second build language or shell expansion. */
    char *args[] = {"cc", "-std=c11", "-O2", "-g", "-Wall", "-Wextra",
        "-Werror", "-pedantic", "-I.", "-o", (char *)output,
        (char *)a, (char *)b, (char *)c, NULL};
    return run(args);
}

static int tools(void)
{
    return RUN("cc", "-std=c11", "-O2", "-g", "-Wall", "-Wextra", "-Werror",
        "-pedantic", "-I.", "-o", "out/p80h", "tools/host.c", "tools/image.c",
        "tools/usb.c", "tools/net.c", "tools/command.c", "tools/get.c", "tools/put.c",
        "tools/gpt.c", "tools/gpt-cli.c") ||
        compile("out/policy", "tools/policy.c", NULL, NULL) ||
        RUN("cc", "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I.",
            "-o", "out/sitegen", "tools/site.c", "tools/records.c", "tools/table.c",
            "tools/html.c", "tools/inline.c", "tools/markdown.c", "tools/library.c",
            "tools/feature.c", "tools/template.c", "tools/maps.c", "tools/graph-data.c",
            "tools/graph-svg.c", "tools/feature-graph.c", "tools/gallery.c", "tools/summary.c") ||
        compile("out/package", "tools/package.c", NULL, NULL);
}

int main(int argc, char **argv)
{
    /* Refuse accidental local builds; the PC owns USB and transfers only. */
    const char *codespaces = getenv("CODESPACES");
    const char *actions = getenv("GITHUB_ACTIONS");
    const char *target = argc == 2 ? argv[1] : "help";
    if (!strcmp(target, "help")) {
        puts("Remote only: ./build {test|site|package|tools|kernel-audit}");
        return 0;
    }
    if ((!codespaces || strcmp(codespaces, "true")) &&
        (!actions || strcmp(actions, "true"))) {
        fputs("Builds run in Codespaces or Actions only.\n", stderr);
        return 1;
    }
    if (strcmp(target, "test") && strcmp(target, "site") &&
        strcmp(target, "package") && strcmp(target, "tools") && strcmp(target, "kernel-audit")) return 2;
    if (mkdir("out", 0755) && errno != EEXIST) return 1;
    if (!strcmp(target, "kernel-audit"))
        return compile("out/kernel-lab", "tools/kernel.c", "tools/table.c", NULL) || RUN("out/kernel-lab");
    if (tools() || RUN("out/policy")) return 1;
    if (!strcmp(target, "tools")) return 0;
    if (compile("out/test", "tests/main.c", "src/boot/menu.c", "tools/image.c") ||
        RUN("out/test")) return 1;
    if (compile("out/native-test", "tests/native.c", "src/boot/native-image.c", "src/boot/native-layout.c") ||
        RUN("out/native-test")) return 1;
    if (compile("out/gpt-test", "tests/gpt.c", "tools/gpt.c", NULL) || RUN("out/gpt-test")) return 1;
    if (RUN("cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I.",
        "-o", "out/transport-test", "tests/transport.c", "tools/net.c",
        "tools/command.c", "tools/get.c") || RUN("out/transport-test")) return 1;
    if (RUN("out/sitegen")) return 1;
    if (RUN("cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I.",
        "-o", "out/web-test", "tests/web.c", "tools/table.c", "tools/html.c",
        "tools/inline.c", "tools/markdown.c", "tools/graph-data.c") || RUN("out/web-test")) return 1;
    if (RUN("cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I.",
        "-o", "out/upload-test-bin", "tests/upload.c", "tools/net.c", "tools/put.c") ||
        RUN("out/upload-test-bin")) return 1;
    if (RUN("cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I.",
        "-o", "out/linkcheck", "tools/links.c", "tools/html.c", "tools/table.c") ||
        RUN("out/linkcheck")) return 1;
    if (!strcmp(target, "package")) return RUN("out/package");
    return 0;
}
