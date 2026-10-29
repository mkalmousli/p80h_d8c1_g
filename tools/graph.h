/* SPDX-License-Identifier: MIT */
#ifndef P80H_GRAPH_H
#define P80H_GRAPH_H
#include "web.h"
struct graph_node { char id[64], label[128], state[32]; int depth, x, y; };
struct graph_edge { unsigned from, to; };
struct graph {
    struct graph_node nodes[64]; struct graph_edge edges[256];
    unsigned count, edge_count;
};
int graph_load(struct graph *, const char *path);
void graph_draw(FILE *, struct graph *, const char *title);
#endif
