/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <string.h>

/* Callbacks consume one row synchronously; pointers expire at the next read. */
int table_read(const char *path, size_t columns, row_fn row, void *context)
{
    FILE *f = fopen(path, "r");
    char line[4096], *fields[12];
    int failed = 0;
    unsigned line_number = 0, count = 0;
    if (!f || !columns || columns > 12) { if (f) fclose(f); return 1; }
    while (fgets(line, sizeof(line), f)) {
        line_number++;
        if (line[0] == '#' || line[0] == '\n') continue;
        char *p = line;
        for (size_t i = 0; i < columns; i++) {
            fields[i] = p;
            p = strchr(p, i == columns - 1 ? '\n' : '|');
            if (!p) { failed = 1; break; }
            *p++ = 0;
            if (!*fields[i] || strchr(fields[i], '\r')) { failed = 1; break; }
        }
        if (failed || *p || strchr(fields[columns - 1], '|') || row(fields, columns, context)) {
            failed = 1; break;
        }
        count++;
    }
    failed |= ferror(f) || !count;
    fclose(f);
    if (failed) fprintf(stderr, "%s:%u: invalid data record\n", path, line_number);
    return failed;
}
