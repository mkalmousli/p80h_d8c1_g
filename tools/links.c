/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Generated pages use a common base, so every internal URL maps to out/site/. */
static int target(const char *url, const char *source)
{
    if (!*url || url[0] == '#' || !strncmp(url, "https://", 8) ||
        !strncmp(url, "http://", 7)) return 0;
    if (url[0] == '/') {
        size_t base = strlen(project.base);
        if (strncmp(url, project.base, base)) return 1;
        url += base;
    }
    char path[2048];
    size_t length = strcspn(url, "#?");
    if (!length) return 0;
    int n = snprintf(path, sizeof(path), "out/site/%.*s", (int)length, url);
    struct stat st;
    if (n < 0 || (size_t)n >= sizeof(path) || stat(path, &st)) {
        fprintf(stderr, "Broken link: %s -> %s\n", source, url); return 1;
    }
    return 0;
}

static int page(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return 1;
    if (fseek(f, 0, SEEK_END)) { fclose(f); return 1; }
    long size = ftell(f);
    if (size < 0 || size > 1024 * 1024) { fclose(f); return 1; }
    rewind(f);
    char *text = malloc((size_t)size + 1);
    if (!text) { fclose(f); return 1; }
    int failed = fread(text, 1, (size_t)size, f) != (size_t)size;
    fclose(f); text[size] = 0;
    for (char *p = text; *p; p++) {
        size_t key = !strncmp(p, "href=", 5) ? 5 : !strncmp(p, "src=", 4) ? 4 : 0;
        if (!key) continue;
        char *value = p + key, quote = *value == '"' || *value == '\'' ? *value++ : 0;
        char *end = value;
        while (*end && (quote ? *end != quote : !isspace((unsigned char)*end) && *end != '>')) end++;
        char saved = *end; *end = 0;
        failed |= target(value, path); *end = saved;
        if (!saved) break;
        p = end;
    }
    free(text); return failed;
}

static int walk(const char *path)
{
    DIR *dir = opendir(path);
    if (!dir) return 1;
    struct dirent *e;
    int failed = 0;
    while ((e = readdir(dir))) {
        if (e->d_name[0] == '.') continue;
        char child[2048];
        int n = snprintf(child, sizeof(child), "%s/%s", path, e->d_name);
        struct stat st;
        if (n < 0 || (size_t)n >= sizeof(child) || stat(child, &st)) { failed = 1; continue; }
        if (S_ISDIR(st.st_mode)) failed |= walk(child);
        else if (strstr(e->d_name, ".html")) failed |= page(child);
    }
    closedir(dir); return failed;
}

int main(void)
{
    if (project_load() || walk("out/site")) return 1;
    puts("Site links: all generated internal links and image sources resolve.");
    return 0;
}
