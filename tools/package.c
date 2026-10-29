/* SPDX-License-Identifier: MIT */
#include "run.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char **argv)
{
    int device = argc == 2 && !strcmp(argv[1], "--device");
    if (argc != 1 && !device) return 2;
    if (mkdir("out/release", 0755) && errno != EEXIST) return 1;
    FILE *f = fopen("out/release/BUILD.txt", "w");
    if (!f) return 1;
    fputs("P80h tools / experimental / not replacement boot firmware\n"
          "Host: Linux x86_64 + system glibc; code license: MIT\n"
          "Protocol: P80H/1.0 binary, USB TCP 4445; no silent legacy downgrade\n"
          "Sources: source.tar; compile entry point: build.c\n"
          "No stock images, vendor firmware or device identity included.\n", f);
    if (device) fputs("Device server: static ARM64 Linux, musl 1.2.5 (source/license included).\n", f);
    if (fclose(f)) return 1;
    if (RUN("git", "archive", "--format=tar", "--output=out/release/source.tar", "HEAD") ||
        RUN("cp", "out/p80h", "LICENSE", "NOTICE", "out/release/")) return 1;
    if (device && (RUN("cp", "out/p80h-link-aarch64", "out/release/") ||
        RUN("cp", "local/musl-source/COPYRIGHT", "out/release/MUSL-COPYRIGHT") ||
        RUN("cp", "local/musl.tar.gz", "out/release/musl-1.2.5.tar.gz"))) return 1;
    int fd = open("out/release/REVISION", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0) return 1;
    close(fd);
    if (RUN("git", "rev-parse", "HEAD")) return 1;
    fd = open("out/release/TOOLCHAIN.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0) return 1;
    close(fd);
    if (RUN("cc", "--version")) return 1;
    if (chdir("out/release")) return 1;
    fd = open("SHA256SUMS", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0) return 1;
    close(fd);
    if (RUN("sha256sum", "p80h", "source.tar", "LICENSE", "NOTICE", "BUILD.txt",
            "REVISION", "TOOLCHAIN.txt")) return 1;
    return device ? RUN("sha256sum", "p80h-link-aarch64", "MUSL-COPYRIGHT", "musl-1.2.5.tar.gz") : 0;
}
