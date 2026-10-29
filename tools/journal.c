/* SPDX-License-Identifier: MIT */
#include "web.h"
#include <string.h>

struct story { FILE *out; char previous[32]; };

/* Entries are ordered UTC events; prose records discoveries rather than fiction. */
static int chapter(char **v, size_t n, void *context)
{
    (void)n;
    struct story *s = context;
    if (strlen(v[0]) != 20 || v[0][10] != 'T' || v[0][19] != 'Z' ||
        strcmp(s->previous, v[0]) > 0 || strncmp(v[3], "docs/", 5) || strstr(v[3], "..")) return 1;
    if (strcmp(v[7], "verified") && strcmp(v[7], "implemented") && strcmp(v[7], "blocked")) return 1;
    strcpy(s->previous, v[0]);
    fputs("<article class=chapter><div class=chapter-meta><time datetime=\"", s->out); html(s->out, v[0]);
    fputs("\">", s->out); html(s->out, v[0]); fputs("</time><span>Run ", s->out); html(s->out, v[1]);
    fputs("</span><span class=state data-state=", s->out); html(s->out, v[7]);
    fputs(">", s->out); html(s->out, v[7]); fputs("</span></div><h2>", s->out); html(s->out, v[2]);
    const char *labels[] = {"The question", "What we learned", "Where this leads"};
    for (unsigned i = 0; i < 3; i++) {
        fputs(i ? "</p><h3>" : "</h2><h3>", s->out); html(s->out, labels[i]);
        fputs("</h3><p>", s->out); html(s->out, v[4 + i]);
    }
    fputs("</p><a href=\"files/", s->out); html(s->out, v[3]);
    fputs(".html\">Read the chapter and evidence →</a></article>", s->out);
    return ferror(s->out);
}

int journal_generate(void)
{
    FILE *out = fopen("out/site/journal.html", "w");
    if (!out) return 1;
    page_head(out, "The bring-up journal");
    fputs("<p class=eyebrow>CHRONOLOGICAL FIELD NOTES / UTC</p><h1>A machine, understood step by step.</h1>"
          "<p>Follow the questions, measurements and setbacks. Timestamps identify the "
          "recorded milestone; each chapter separates observations from goals.</p><div class=story>", out);
    struct story s = {.out = out, .previous = {0}};
    int failed = table_read("data/journal.tsv", 8, chapter, &s);
    fputs("</div>", out); page_foot(out);
    failed |= ferror(out); failed |= fclose(out) != 0; return failed;
}
