/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <string.h>

struct counts { unsigned values[5], total; };
static const char *states[] = {"planned", "observed", "implemented", "verified", "blocked"};

static int count(char **v, size_t n, void *context)
{
    (void)n;
    struct counts *c = context;
    for (unsigned i = 0; i < 5; i++) if (!strcmp(states[i], v[2])) {
        c->values[i]++; c->total++; return 0;
    }
    return 1;
}

int summary_entries(FILE *out)
{
    struct counts c = {0};
    if (table_read("data/features.tsv", 8, count, &c)) return 1;
    fputs("<div class=counts aria-label=\"Feature evidence summary\">", out);
    fprintf(out, "<div><b>%u</b><span>features tracked</span></div>", c.total);
    for (unsigned i = 0; i < 5; i++) {
        fprintf(out, "<div data-state=%s><b>%u</b><span>%s</span></div>",
                states[i], c.values[i], states[i]);
    }
    fputs("</div>", out);
    return ferror(out);
}

static int specification(char **v, size_t n, void *context)
{
    (void)n;
    FILE *out = context;
    fputs("<tr><th scope=row>", out); html(out, v[0]);
    fputs("</th><td>", out); html(out, v[1]); fputs("</td></tr>", out);
    return ferror(out);
}

int device_entries(FILE *out)
{
    fputs("<div class=table-scroll><table><caption class=sr-only>Device facts and targets</caption><tbody>", out);
    int failed = table_read("data/device.tsv", 2, specification, out);
    fputs("</tbody></table></div>", out);
    return failed || ferror(out);
}
