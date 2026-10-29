/* SPDX-License-Identifier: MIT */
#include "records.h"
#include "run.h"
#include "web.h"
#include <string.h>
#include <sys/stat.h>
int maps_generate(void);
int template_expand(FILE *, FILE *);

/* Deterministic static publication: templates + data + tracked text, no network. */
int main(void)
{
    if (project_load()) return 1;
    if (make_parents("out/site/index.html")) return 1;
    if (RUN_TO("out/site/revision.txt", "git", "log", "-1", "--format=%h %cs")) return 1;
    FILE *revision = fopen("out/site/revision.txt", "r");
    if (!revision || !fgets(project.revision, sizeof(project.revision), revision)) return 1;
    fclose(revision); project.revision[strcspn(project.revision, "\r\n")] = 0;
    if (RUN("cp", "-R", "site/assets", "site/style.css", "site/reference.css", "site/.nojekyll", "out/site/")) return 1;
    FILE *input = fopen("site/index.html", "r");
    FILE *output = fopen("out/site/index.html", "w");
    FILE *docs = fopen("out/progress.md", "w");
    if (!input || !output || !docs) return 1;
    int failed = template_expand(input, output);
    fputs("# Progress\n\nGenerated from data/progress.tsv.\n\n"
          "| Milestone | State | Evidence / next gate |\n|---|---|---|\n", docs);
    failed |= records(docs, 1, 'M');
    fputs("\n| Hardware | State | Evidence / next gate |\n|---|---|---|\n", docs);
    failed |= feature_markdown(docs);
    failed |= ferror(input) || ferror(output) || ferror(docs);
    fclose(input); failed |= fclose(output) != 0; failed |= fclose(docs) != 0;
    if (failed || feature_pages(NULL) || library_pages() || maps_generate() || gallery_generate() || journal_generate()) return 1;
    puts("Site: generated feature pages, repository library, progress and SVG maps.");
    return 0;
}
