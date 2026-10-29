/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static int check(const char *path)
{
    struct stat st;
    if (lstat(path, &st)) return 1;
    if (S_ISLNK(st.st_mode)) { fprintf(stderr, "Symlink: %s\n", path); return 1; }
    if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(path);
        if (!d) return 1;
        struct dirent *e;
        int failed = 0;
        while ((e = readdir(d))) {
            if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..") ||
                !strcmp(e->d_name, ".git") || !strcmp(e->d_name, "out") ||
                !strcmp(e->d_name, "local") || !strcmp(e->d_name, "build")) continue;
            char child[1024];
            int n = snprintf(child, sizeof(child), "%s/%s", path, e->d_name);
            failed |= n < 0 || (size_t)n >= sizeof(child) ? 1 : check(child);
        }
        closedir(d);
        return failed;
    }
    const char *extension = strrchr(path, '.');
    const char *allowed[] = {".c", ".h", ".md", ".json", ".yml", ".yaml",
        ".css", ".html", ".svg", ".tsv", ".gitignore", ".nojekyll", NULL};
    const char *name = strrchr(path, '/');
    name = name ? name + 1 : path;
    int ok = !strcmp(name, "LICENSE") || !strcmp(name, "NOTICE");
    for (size_t i = 0; extension && allowed[i]; i++) ok |= !strcmp(extension, allowed[i]);
    if (!ok || st.st_size > 128 * 1024 || strstr(name, "hwparam")) {
        fprintf(stderr, "Disallowed public file: %s\n", path); return 1;
    }
    FILE *f = fopen(path, "rb");
    if (!f) return 1;
    unsigned lines = 0;
    int ch, last = '\n', failed = 0;
    while ((ch = fgetc(f)) != EOF) {
        if (!ch) failed = 1;
        if (ch == '\n') lines++;
        last = ch;
    }
    if (last != '\n') lines++;
    failed |= ferror(f) || lines > 100;
    fclose(f);
    if (failed) fprintf(stderr, "Not text or over 100 lines: %s (%u)\n", path, lines);
    return failed;
}

int main(void)
{
    int result = check(".");
    if (!result) puts("Source policy: text-only allowlist, no symlinks, <=100 lines/file.");
    return result;
}
