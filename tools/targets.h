/* SPDX-License-Identifier: MIT */
#ifndef P80H_TARGETS_H
#define P80H_TARGETS_H
#include "run.h"
#define WIRE_SOURCES "src/link/codec.c", "src/link/io.c"
#define SERVER_SOURCES "src/link/server.c", "src/link/status.c", "src/link/exec.c", \
    "src/link/files.c", "src/link/upload.c"
#define CLIENT_SOURCES "tools/link-session.c", "tools/link-cli.c", \
    "tools/link-download.c", "tools/link-upload.c"
#define ROM_SOURCES "src/rom/descriptors.c", "src/rom/probe.c", "src/rom/reset.c"

/* A single compiler contract for host tools, tests and static device binaries. */
static int compile_sources(const char *cc, const char *output, int standalone,
                           char *const sources[])
{
    char *args[128] = {(char *)cc, "-std=c11", "-Os", "-g", "-Wall", "-Wextra",
        "-Werror", "-pedantic", "-I.", "-o", (char *)output};
    size_t count = 11;
    if (standalone) { args[count++] = "-static"; args[count++] = "-s"; }
    for (size_t i = 0; sources[i]; i++) {
        if (count == 127) return 1;
        args[count++] = sources[i];
    }
    args[count] = NULL;
    return run(args);
}
#define BUILD(out, ...) compile_sources("cc", out, 0, (char *const[]){__VA_ARGS__, NULL})
#define DEVICE(out, ...) compile_sources("out/musl/bin/musl-gcc", out, 1, (char *const[]){__VA_ARGS__, NULL})

static int build_tools(void)
{
    return BUILD("out/p80h", "tools/entry.c", "tools/host.c", "tools/image.c",
        "tools/usb.c", "tools/net.c", "tools/command.c", "tools/get.c", "tools/put.c",
        "tools/gpt.c", "tools/gpt-cli.c", "tools/rom-find.c", "tools/rom-usb.c", "tools/rom-info.c",
        ROM_SOURCES, CLIENT_SOURCES, WIRE_SOURCES, "src/device/features.c") ||
        BUILD("out/policy", "tools/policy.c") ||
        BUILD("out/package", "tools/package.c");
}

static int build_tests(void)
{
    return BUILD("out/test", "tests/main.c", "src/boot/menu.c", "tools/image.c") || RUN("out/test") ||
        BUILD("out/native-test", "tests/native.c", "src/boot/native-image.c", "src/boot/native-layout.c") || RUN("out/native-test") ||
        BUILD("out/gpt-test", "tests/gpt.c", "tools/gpt.c") || RUN("out/gpt-test") ||
        BUILD("out/transport-test", "tests/transport.c", "tools/net.c", "tools/command.c", "tools/get.c") || RUN("out/transport-test") ||
        BUILD("out/upload-test-bin", "tests/upload.c", "tools/net.c", "tools/put.c") || RUN("out/upload-test-bin") ||
        BUILD("out/wire-test", "tests/wire.c", WIRE_SOURCES, SERVER_SOURCES) || RUN("out/wire-test") ||
        BUILD("out/link-transfer-test", "tests/link-transfer.c", "tools/link-session.c", "tools/net.c",
            "tools/link-download.c", "tools/link-upload.c", WIRE_SOURCES, SERVER_SOURCES) || RUN("out/link-transfer-test") ||
        BUILD("out/rom-test", "tests/rom.c", ROM_SOURCES) || RUN("out/rom-test") ||
        BUILD("out/rom-reset-test", "tests/rom-reset.c", "src/rom/reset.c") || RUN("out/rom-reset-test");
}
#endif
