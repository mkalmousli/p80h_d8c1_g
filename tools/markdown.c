/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <ctype.h>
#include <string.h>

static void table_row(FILE *out, char *line, const char *source)
{
    fputs("<tr>", out);
    char *p = line + 1;
    while (*p) {
        char *end = strchr(p, '|');
        if (!end) break;
        *end = 0;
        fputs("<td>", out); inline_markdown(out, p, source); fputs("</td>", out);
        p = end + 1;
    }
    fputs("</tr>\n", out);
}

/* Unsupported Markdown remains readable escaped text; raw HTML never executes. */
int markdown(FILE *input, FILE *out, const char *source)
{
    char line[8192];
    int fence = 0, table = 0, list = 0;
    while (fgets(line, sizeof(line), input)) {
        if (!strchr(line, '\n') && !feof(input)) return 1;
        line[strcspn(line, "\r\n")] = 0;
        if (!strncmp(line, "```", 3)) {
            if (table) { fputs("</tbody></table></div>", out); table = 0; }
            if (list) { fputs("</ul>", out); list = 0; }
            fputs(fence ? "</code></pre>\n" : "<pre><code>", out);
            fence = !fence; continue;
        }
        if (fence) { html(out, line); fputc('\n', out); continue; }
        if (line[0] == '|') {
            if (strspn(line, "| :-") == strlen(line)) continue;
            if (!table) {
                if (list) { fputs("</ul>", out); list = 0; }
                fputs("<div class=table-scroll><table><tbody>", out); table = 1;
            }
            table_row(out, line, source); continue;
        }
        if (table) { fputs("</tbody></table></div>", out); table = 0; }
        int bullet = !strncmp(line, "- ", 2) || !strncmp(line, "* ", 2);
        size_t digits = strspn(line, "0123456789");
        if (digits && line[digits] == '.' && line[digits + 1] == ' ') bullet = 1;
        if (bullet) {
            if (!list) { fputs("<ul>", out); list = 1; }
            fputs("<li>", out); inline_markdown(out, line + (digits ? digits + 2 : 2), source);
            fputs("</li>\n", out); continue;
        }
        if (list) { fputs("</ul>", out); list = 0; }
        unsigned heading = 0;
        while (heading < 6 && line[heading] == '#') heading++;
        if (heading && line[heading] == ' ') {
            fprintf(out, "<h%u>", heading); inline_markdown(out, line + heading + 1, source);
            fprintf(out, "</h%u>\n", heading); continue;
        }
        if (*line) { fputs("<p>", out); inline_markdown(out, line, source); fputs("</p>\n", out); }
    }
    if (fence) fputs("</code></pre>", out);
    if (table) fputs("</tbody></table></div>", out);
    if (list) fputs("</ul>", out);
    return ferror(input) || ferror(out);
}
