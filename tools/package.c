/* SPDX-License-Identifier: MIT */
#include "run.h"
#include <fcntl.h>
#include <sys/stat.h>

int main(void)
{
    if (mkdir("out/release", 0755) && errno != EEXIST) return 1;
    FILE *f = fopen("out/release/BUILD.txt", "w");
    if (!f) return 1;
    fputs("P80h host tools / experimental / not flashable firmware\n"
          "License: MIT; runtime: Linux x86_64 + glibc\n"
          "Sources: source.tar; compile entry point: build.c\n"
          "No stock images, vendor firmware or device identity included.\n", f);
    if (fclose(f)) return 1;
    if (RUN("git", "archive", "--format=tar", "--output=out/release/source.tar", "HEAD") ||
        RUN("cp", "out/p80h", "LICENSE", "NOTICE", "out/release/")) return 1;
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
    return RUN("sha256sum", "p80h", "source.tar", "LICENSE", "NOTICE", "BUILD.txt",
               "REVISION", "TOOLCHAIN.txt");
}
