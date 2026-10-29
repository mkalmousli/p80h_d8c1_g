/* SPDX-License-Identifier: MIT */
#include "gpt.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/* Inspect the primary snapshot only; labels/geometry are public, GUIDs are not. */
int gpt_inspect(const char *path)
{
    enum { LIMIT = 128 * 1024 };
    unsigned char *data = malloc(LIMIT + 1);
    FILE *input = fopen(path, "rb");
    if (!data || !input) { free(data); if (input) fclose(input); return 1; }
    size_t size = fread(data, 1, LIMIT + 1, input);
    struct partition_table table;
    int failed = ferror(input) || size > LIMIT || !gpt_parse(data, size, &table);
    fclose(input); free(data);
    if (failed) { fputs("Invalid, oversized or unsupported primary GPT snapshot.\n", stderr); return 1; }
    puts("Validated primary GPT: header/table CRC32 and partition bounds. GUIDs omitted.");
    for (unsigned i = 0; i < table.count; i++) {
        const struct partition *p = &table.parts[i];
        printf("p%u %-24s start=%" PRIu64 " sectors=%" PRIu64 "\n",
               p->slot, p->name, p->first, p->last - p->first + 1);
    }
    return 0;
}
