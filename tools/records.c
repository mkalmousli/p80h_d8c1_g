/* SPDX-License-Identifier: MIT */
#include "records.h"
#include <string.h>

int records(FILE *out, int markdown, char kind)
{
    FILE *input = fopen("data/progress.tsv", "r");
    char line[1024];
    unsigned count = 0;
    if (!input) return 1;
    while (fgets(line, sizeof(line), input)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        char *fields[5], *p = line;
        for (unsigned i = 0; i < 5; i++) {
            fields[i] = p;
            p = strchr(p, i == 4 ? '\n' : '|');
            if (!p) { fclose(input); return 1; }
            *p++ = 0;
            if (!*fields[i] || strpbrk(fields[i], "<>&\"`")) { fclose(input); return 1; }
        }
        if (*p || strchr(fields[4], '|') || strlen(fields[0]) != 1 ||
            !strchr("MH", fields[0][0])) { fclose(input); return 1; }
        if (strcmp(fields[3], "planned") && strcmp(fields[3], "implemented") &&
            strcmp(fields[3], "verified") && strcmp(fields[3], "blocked")) {
            fclose(input); return 1;
        }
        if (fields[0][0] != kind) continue;
        count++;
        if (markdown)
            fprintf(out, "| %s | %s | %s |\n", fields[2], fields[3], fields[4]);
        else if (kind == 'M')
            fprintf(out, "<li><span class=number>%s</span><div><h3>%s</h3>"
                "<p>%s</p></div><span class=state>%s</span></li>\n",
                fields[1], fields[2], fields[4], fields[3]);
        else
            fprintf(out, "<tr><th scope=row>%s</th><td>%s</td><td>%s</td></tr>\n",
                fields[2], fields[3], fields[4]);
    }
    int failed = ferror(input) || ferror(out) || !count;
    fclose(input);
    return failed;
}
