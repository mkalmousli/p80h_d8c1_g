/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <string.h>

/* Generate one detail page and one index row from each hardware record. */
static int feature(char **v, size_t n, void *context)
{
    (void)n;
    FILE *index = context;
    if (strspn(v[0], "abcdefghijklmnopqrstuvwxyz0123456789-") != strlen(v[0])) return 1;
    const char *states[] = {"planned", "observed", "implemented", "verified", "blocked"};
    int valid = 0;
    for (size_t i = 0; i < 5; i++) valid |= !strcmp(v[2], states[i]);
    if (!valid) return 1;
    char path[512];
    int length = snprintf(path, sizeof(path), "out/site/hardware/%s.html", v[0]);
    if (length < 0 || (size_t)length >= sizeof(path) || make_parents(path)) return 1;
    FILE *out = fopen(path, "w");
    if (!out) return 1;
    page_head(out, v[1]);
    fputs("<p class=eyebrow>HARDWARE / DRIVER RECORD</p><h1>", out); html(out, v[1]);
    fputs("</h1><p><span class=state data-state=\"", out); html(out, v[2]);
    fputs("\">", out); html(out, v[2]); fputs("</span></p><dl class=driver>", out);
    const char *labels[] = {"Component", "Driver / implementation", "Evidence",
                            "Next acceptance gate", "Dependency"};
    for (size_t i = 0; i < 5; i++) {
        fputs("<dt>", out); html(out, labels[i]); fputs("</dt><dd>", out);
        html(out, v[i + 3]); fputs("</dd>", out);
    }
    fputs("</dl><p>Observed means a driver is bound; it does not prove the hardware works. "
          "Verified applies only to the exact evidence described.</p>"
          "<p><a href=files/data/features.tsv.html>View versioned record</a> · "
          "<a href=hardware.html>All features</a> · <a href=diagrams.html>Dependency maps</a></p>", out);
    page_foot(out);
    int failed = ferror(out); failed |= fclose(out) != 0;
    fputs("<tr><th scope=row><a href=\"hardware/", index); html(index, v[0]);
    fputs(".html\">", index); html(index, v[1]); fputs("</a></th><td><span class=state data-state=\"", index);
    html(index, v[2]); fputs("\">", index); html(index, v[2]);
    fputs("</span></td><td>", index); html(index, v[4]); fputs("</td></tr>\n", index);
    return failed || ferror(index);
}

int feature_pages(FILE *summary)
{
    FILE *f = fopen("out/site/hardware.html", "w");
    if (!f) return 1;
    page_head(f, "Hardware and drivers");
    fputs("<p class=eyebrow>DEVICE INVENTORY</p><h1>Hardware & drivers.</h1>"
          "<p>Every known capability, including unknown and blocked paths. "
          "Select a feature for its driver, evidence, dependency and acceptance gate.</p>"
          "<div class=legend><span data-state=planned>planned</span>"
          "<span data-state=observed>observed</span><span data-state=implemented>implemented</span>"
          "<span data-state=verified>verified</span><span data-state=blocked>blocked</span></div>"
          "<div class=table-scroll><table><thead><tr><th>Feature</th><th>State</th>"
          "<th>Driver / implementation</th></tr></thead><tbody>", f);
    int failed = table_read("data/features.tsv", 8, feature, f);
    fputs("</tbody></table></div>", f); page_foot(f);
    failed |= ferror(f); failed |= fclose(f) != 0;
    if (summary) fputs("<p><a class=button href=hardware.html>Browse every feature & driver →</a></p>", summary);
    return failed;
}

/* The downloadable Markdown summary shares the complete hardware inventory. */
static int markdown_row(char **v, size_t n, void *out)
{
    (void)n;
    return fprintf(out, "| %s | %s | %s |\n", v[1], v[2], v[5]) < 0;
}

int feature_markdown(FILE *out)
{
    return table_read("data/features.tsv", 8, markdown_row, out);
}
