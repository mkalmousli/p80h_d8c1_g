/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <string.h>

/* Image metadata is public evidence; unreviewed captures remain outside git. */
static int capture(char **v, size_t n, void *context)
{
    (void)n;
    FILE *out = context;
    if (strncmp(v[2], "assets/captures/", 16) || strstr(v[2], "..") ||
        strspn(v[2], "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/." ) != strlen(v[2])) return 1;
    fputs("<figure class=capture><a href=\"", out); html(out, v[2]);
    fputs("\"><img width=800 height=1280 src=\"", out); html(out, v[2]);
    fputs("\" alt=\"", out); html(out, v[1]); fputs("\"></a><figcaption><strong>", out);
    html(out, v[1]); fputs("</strong><br>Run ", out); html(out, v[0]);
    fputs(" · ", out); html(out, v[4]); fputs(" · source ", out); html(out, v[3]);
    fputs("<br>", out); html(out, v[5]); fputs("</figcaption></figure>\n", out);
    return ferror(out);
}

int gallery_entries(FILE *out)
{
    return table_read("data/captures.tsv", 6, capture, out);
}

int gallery_generate(void)
{
    FILE *out = fopen("out/site/gallery.html", "w");
    if (!out) return 1;
    page_head(out, "Visual field log");
    fputs("<p class=eyebrow>REAL DEVICE EVIDENCE</p><h1>The visual field log.</h1>"
          "<p>Reviewed captures from the connected tablet. Every entry names its "
          "source and what it proves; concepts are kept separate.</p>", out);
    int failed = gallery_entries(out);
    page_foot(out); failed |= ferror(out); failed |= fclose(out) != 0;
    return failed;
}
