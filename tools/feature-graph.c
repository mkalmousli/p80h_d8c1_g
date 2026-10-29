/* SPDX-License-Identifier: MIT */
#include "feature-map.h"
#include <string.h>

static int collect(char **v, size_t count, void *context)
{
    (void)count;
    struct inventory *s = context;
    unsigned i = s->graph.count;
    if (i == 64 || strlen(v[0]) >= 64 || strlen(v[1]) >= 128 ||
        strlen(v[2]) >= 32 || strlen(v[7]) >= 64) return 1;
    strcpy(s->graph.nodes[i].id, v[0]); strcpy(s->graph.nodes[i].label, v[1]);
    strcpy(s->graph.nodes[i].state, v[2]); strcpy(s->dependency[i], v[7]);
    s->graph.count++;
    return 0;
}

/* Resolve all IDs before layout; DFS rejects cycles and missing dependencies. */
static int depth(struct inventory *s, unsigned i)
{
    if (s->marks[i] == 2) return 0;
    if (s->marks[i] == 1) return 1;
    s->marks[i] = 1;
    if (strcmp(s->dependency[i], "none")) {
        unsigned parent;
        for (parent = 0; parent < s->graph.count; parent++)
            if (!strcmp(s->dependency[i], s->graph.nodes[parent].id)) break;
        if (parent == s->graph.count || depth(s, parent)) return 1;
        s->graph.nodes[i].depth = s->graph.nodes[parent].depth + 1;
        if (s->graph.edge_count == 256) return 1;
        s->graph.edges[s->graph.edge_count++] = (struct graph_edge){parent, i};
    }
    s->marks[i] = 2;
    return 0;
}

int feature_graph(void)
{
    struct inventory s = {0};
    if (table_read("data/features.tsv", 8, collect, &s)) return 1;
    for (unsigned i = 0; i < s.graph.count; i++) if (depth(&s, i)) return 1;
    return feature_map(&s);
}
