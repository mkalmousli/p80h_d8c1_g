/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

struct project project;

/* All dynamic content passes through this function, including attributes. */
void html(FILE *out, const char *text)
{
    for (; *text; text++) {
        switch (*text) {
        case '&': fputs("&amp;", out); break;
        case '<': fputs("&lt;", out); break;
        case '>': fputs("&gt;", out); break;
        case '"': fputs("&quot;", out); break;
        case '\'': fputs("&#39;", out); break;
        default: fputc((unsigned char)*text, out);
        }
    }
}

static int setting(char **v, size_t n, void *unused)
{
    (void)n; (void)unused;
    struct { const char *key; char *dest; size_t size; } keys[] = {
        {"name", project.name, sizeof(project.name)},
        {"summary", project.summary, sizeof(project.summary)},
        {"base", project.base, sizeof(project.base)},
        {"repo", project.repo, sizeof(project.repo)}};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        if (strcmp(v[0], keys[i].key)) continue;
        if (*keys[i].dest || strlen(v[1]) >= keys[i].size) return 1;
        strcpy(keys[i].dest, v[1]); return 0;
    }
    return 1;
}

int project_load(void)
{
    if (table_read("data/project.tsv", 2, setting, NULL)) return 1;
    return !*project.name || !*project.summary || project.base[0] != '/' ||
        strpbrk(project.base, "\"<>& ") || strncmp(project.repo, "https://github.com/", 19);
}

void page_head(FILE *f, const char *title)
{
    fputs("<!doctype html><html lang=en><head><meta charset=utf-8>"
          "<meta name=viewport content=\"width=device-width,initial-scale=1\"><base href=\"", f);
    html(f, project.base); fputs("\"><title>", f); html(f, title);
    fputs(" / ", f); html(f, project.name);
    fputs("</title><link rel=icon href=assets/mark.svg><link rel=stylesheet href=style.css>"
          "</head><body><a class=skip href=#main>Skip to content</a><header class=wrap>"
          "<a class=brand href=./><img src=assets/mark.svg width=28 height=28 alt=\"\">", f);
    html(f, project.name);
    fputs("</a><nav aria-label=Main><a href=./#progress>Progress</a>"
          "<a href=hardware.html>Hardware</a><a href=library.html>Library</a>"
          "<a href=diagrams.html>Maps</a></nav></header><main id=main class=\"wrap document\">", f);
}

void page_foot(FILE *f)
{
    fputs("</main><footer class=wrap><span>Generated from versioned repository data.</span>"
          "<a href=files/NOTICE.html>Sources & licenses</a></footer></body></html>\n", f);
}

/* Only caller-validated relative paths reach this helper. No shell commands. */
int make_parents(const char *file)
{
    char path[1024];
    if (strlen(file) >= sizeof(path)) return 1;
    strcpy(path, file);
    for (char *p = path + 1; *p; p++) if (*p == '/') {
        *p = 0;
        if (mkdir(path, 0755) && errno != EEXIST) return 1;
        *p = '/';
    }
    return 0;
}
