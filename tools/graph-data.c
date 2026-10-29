/* SPDX-License-Identifier: MIT */
#include "graph.h"
#include <string.h>

/* Requiring parents before children makes cycles and unknown dependencies errors. */
static int node(char **v, size_t n, void *context)
{
    (void)n;
    struct graph *g = context;
    if (g->count == 64 || strlen(v[0]) >= 64 || strlen(v[1]) >= 128 || strlen(v[2]) >= 32)
        return 1;
    if (strspn(v[0], "abcdefghijklmnopqrstuvwxyz0123456789-") != strlen(v[0])) return 1;
    for (unsigned i = 0; i < g->count; i++) if (!strcmp(v[0], g->nodes[i].id)) return 1;
    struct graph_node *item = &g->nodes[g->count];
    strcpy(item->id, v[0]); strcpy(item->label, v[1]); strcpy(item->state, v[2]);
    if (strcmp(v[3], "none")) {
        char *parent = v[3];
        while (parent) {
            char *next = strchr(parent, ',');
            if (next) *next++ = 0;
            unsigned i;
            for (i = 0; i < g->count; i++) if (!strcmp(parent, g->nodes[i].id)) break;
            if (i == g->count || g->edge_count == 256) return 1;
            g->edges[g->edge_count++] = (struct graph_edge){i, g->count};
            if (item->depth <= g->nodes[i].depth) item->depth = g->nodes[i].depth + 1;
            parent = next;
        }
    }
    g->count++;
    return 0;
}

int graph_load(struct graph *g, const char *path)
{
    *g = (struct graph){0};
    return table_read(path, 4, node, g);
}
