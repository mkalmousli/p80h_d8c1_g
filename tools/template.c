/* SPDX-License-Identifier: MIT */
#include "web.h"
#include "records.h"
#include <string.h>

/* Every template token is explicit. Unknown or unterminated tokens fail builds. */
static int token(FILE *out, const char *key)
{
    if (!strcmp(key, "milestones")) return records(out, 0, 'M');
    if (!strcmp(key, "hardware")) return records(out, 0, 'H');
    if (!strcmp(key, "captures")) return gallery_entries(out);
    if (!strcmp(key, "counts")) return summary_entries(out);
    if (!strcmp(key, "device")) return device_entries(out);
    const struct { const char *key, *value; } values[] = {
        {"name", project.name}, {"summary", project.summary},
        {"base", project.base}, {"repo", project.repo}};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (strcmp(key, values[i].key)) continue;
        html(out, values[i].value); return ferror(out);
    }
    fprintf(stderr, "Unknown template token: %s\n", key);
    return 1;
}

int template_expand(FILE *input, FILE *out)
{
    int c;
    while ((c = fgetc(input)) != EOF) {
        if (c != '{') { fputc(c, out); continue; }
        if (fgetc(input) != '{') return 1;
        char key[64];
        size_t used = 0;
        while ((c = fgetc(input)) != EOF && c != '}') {
            if (used == sizeof(key) - 1) return 1;
            key[used++] = (char)c;
        }
        if (c != '}' || fgetc(input) != '}') return 1;
        key[used] = 0;
        if (token(out, key)) return 1;
    }
    return ferror(input) || ferror(out);
}
