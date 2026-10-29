/* SPDX-License-Identifier: MIT */
#include "tools/web.h"
#include "tools/graph.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static int accept(char **v, size_t n, void *context)
{
    (void)context;
    return n != 2 || strcmp(v[0], "one") || strcmp(v[1], "two");
}

static void text_file(const char *path, const char *text)
{
    FILE *f = fopen(path, "w");
    assert(f && fputs(text, f) >= 0 && !fclose(f));
}

int main(void)
{
    text_file("out/table-test.tsv", "one|two\n");
    assert(!table_read("out/table-test.tsv", 2, accept, NULL));
    text_file("out/table-test.tsv", "one|two|three\n");
    assert(table_read("out/table-test.tsv", 2, accept, NULL));
    text_file("out/table-test.tsv", "one|two");
    assert(table_read("out/table-test.tsv", 2, accept, NULL));
    text_file("out/graph-test.tsv", "a|first|planned|none\nb|second|planned|a\n");
    struct graph g;
    assert(!graph_load(&g, "out/graph-test.tsv") && g.nodes[1].depth == 1);
    text_file("out/graph-test.tsv", "a|first|planned|b\nb|second|planned|a\n");
    assert(graph_load(&g, "out/graph-test.tsv"));
    text_file("out/graph-test.tsv", "a|first|planned|none\na|duplicate|planned|none\n");
    assert(graph_load(&g, "out/graph-test.tsv"));
    FILE *input = tmpfile(), *output = tmpfile();
    assert(input && output);
    fputs("# Heading\n<script>alert(1)</script>\n[bad](javascript:alert)\n"
          "[doc](recovery.md)\n```c\nint x = 1 < 2;\n```\n", input);
    rewind(input);
    assert(!markdown(input, output, "docs/status.md"));
    rewind(output);
    char result[4096] = {0};
    assert(fread(result, 1, sizeof(result) - 1, output) > 0);
    assert(strstr(result, "<h1>Heading</h1>"));
    assert(strstr(result, "&lt;script&gt;") && !strstr(result, "<script>"));
    assert(!strstr(result, "href=\"javascript:"));
    assert(strstr(result, "files/docs/recovery.md.html"));
    assert(strstr(result, "1 &lt; 2"));
    fclose(input); fclose(output);
    puts("PASS: strict data records, graph dependency errors and safe Markdown rendering");
    return 0;
}
