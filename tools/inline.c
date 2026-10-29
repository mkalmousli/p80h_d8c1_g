/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <string.h>

/* Resolve relative repository links without allowing traversal above its root. */
static void link(FILE *f, const char *target, const char *source)
{
    if (!strncmp(target, "https://", 8) || !strncmp(target, "http://", 7) || target[0] == '#') {
        html(f, target); return;
    }
    if (strchr(target, ':') || target[0] == '/') { fputs("library.html", f); return; }
    char path[1024], normalized[1024] = "", *parts[100];
    size_t depth = 0;
    const char *slash = strrchr(source, '/');
    int n = snprintf(path, sizeof(path), "%.*s%s", slash ? (int)(slash - source + 1) : 0,
                     source, target);
    if (n < 0 || (size_t)n >= sizeof(path)) { fputs("library.html", f); return; }
    char *p = path;
    while (*p) {
        char *end = strchr(p, '/');
        if (end) *end = 0;
        if (!strcmp(p, "..")) { if (!depth) { fputs("library.html", f); return; } depth--; }
        else if (*p && strcmp(p, ".")) {
            if (depth == 100) { fputs("library.html", f); return; }
            parts[depth++] = p;
        }
        if (!end) break;
        p = end + 1;
    }
    for (size_t i = 0; i < depth; i++) {
        if (i) strcat(normalized, "/");
        strcat(normalized, parts[i]);
    }
    fputs("files/", f); html(f, normalized); fputs(".html", f);
}

/* Deliberately small Markdown dialect: links and inline code, escaped fallback. */
void inline_markdown(FILE *f, const char *text, const char *source)
{
    while (*text) {
        const char *end;
        if (*text == '`' && (end = strchr(text + 1, '`'))) {
            fputs("<code>", f);
            while (++text < end) { char c[] = {*text, 0}; html(f, c); }
            fputs("</code>", f); text++; continue;
        }
        if (*text == '[' && (end = strstr(text + 1, "]("))) {
            const char *close = strchr(end + 2, ')');
            if (close && (size_t)(close - end - 2) < 1024) {
                char target[1024];
                memcpy(target, end + 2, (size_t)(close - end - 2));
                target[close - end - 2] = 0;
                fputs("<a href=\"", f); link(f, target, source); fputs("\">", f);
                for (text++; text < end; text++) { char c[] = {*text, 0}; html(f, c); }
                fputs("</a>", f); text = close + 1; continue;
            }
        }
        char c[] = {*text++, 0}; html(f, c);
    }
}
