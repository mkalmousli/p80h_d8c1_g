/* SPDX-License-Identifier: MIT */
#ifndef P80H_WEB_H
#define P80H_WEB_H
#include <stdio.h>
#include <stddef.h>
struct project { char name[128], summary[512], base[128], repo[256], revision[128]; };
extern struct project project;
/* Shared output primitives always escape text; source HTML is never trusted. */
void html(FILE *, const char *);
void page_head(FILE *, const char *title);
void page_foot(FILE *);
int project_load(void);
int make_parents(const char *file);
typedef int (*row_fn)(char **fields, size_t count, void *context);
/* Strict pipe records: bounded fields, exact column count, required newline. */
int table_read(const char *path, size_t columns, row_fn, void *context);
int feature_pages(FILE *summary);
int library_pages(void);
int markdown(FILE *input, FILE *output, const char *source_path);
void inline_markdown(FILE *, const char *, const char *source_path);
int graph_generate(const char *data_path, const char *svg_path, const char *title);
int feature_graph(void);
int gallery_entries(FILE *);
int gallery_generate(void);
int summary_entries(FILE *);
int device_entries(FILE *);
int feature_markdown(FILE *);
#endif
