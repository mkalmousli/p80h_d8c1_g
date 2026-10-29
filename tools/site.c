/* SPDX-License-Identifier: MIT */
#include "records.h"
#include "run.h"
#include <string.h>
#include <sys/stat.h>

int main(void)
{
    if (mkdir("out/site", 0755) && errno != EEXIST) return 1;
    FILE *input = fopen("site/index.html", "r");
    FILE *output = fopen("out/site/index.html", "w");
    FILE *docs = fopen("out/progress.md", "w");
    if (!input || !output || !docs) return 1;
    char line[2048];
    int failed = 0;
    while (fgets(line, sizeof(line), input)) {
        if (!strcmp(line, "{{milestones}}\n")) failed |= records(output, 0, 'M');
        else if (!strcmp(line, "{{hardware}}\n")) failed |= records(output, 0, 'H');
        else if (strstr(line, "{{") || fputs(line, output) == EOF) failed = 1;
    }
    fputs("# Progress\n\nGenerated from data/progress.tsv.\n\n"
          "| Milestone | State | Evidence / next gate |\n|---|---|---|\n", docs);
    failed |= records(docs, 1, 'M');
    fputs("\n| Hardware | State | Evidence / next gate |\n|---|---|---|\n", docs);
    failed |= records(docs, 1, 'H');
    failed |= ferror(input) || ferror(output) || ferror(docs);
    fclose(input);
    failed |= fclose(output) != 0;
    failed |= fclose(docs) != 0;
    if (failed) return 1;
    return RUN("cp", "-R", "site/assets", "site/style.css", "site/.nojekyll", "out/site/");
}
