/* SPDX-License-Identifier: MIT */
#include "graph.h"
#include <string.h>

static const char *color(const char *state)
{
    if (!strcmp(state, "verified")) return "#83c7a6";
    if (!strcmp(state, "implemented")) return "#89b4d0";
    if (!strcmp(state, "observed")) return "#c6bb88";
    if (!strcmp(state, "blocked")) return "#db9c9c";
    return "#94a5a2";
}

/* A deterministic layered DAG layout: no runtime JS or graph engine dependency. */
void graph_draw(FILE *f, struct graph *g, const char *title)
{
    unsigned counts[64] = {0}, used[64] = {0};
    int max_depth = 0;
    for (unsigned i = 0; i < g->count; i++) {
        counts[g->nodes[i].depth]++;
        if (g->nodes[i].depth > max_depth) max_depth = g->nodes[i].depth;
    }
    int width = 1000, height = (max_depth + 1) * 112 + 40;
    for (unsigned i = 0; i < 64; i++)
        if ((int)counts[i] * 260 > width) width = (int)counts[i] * 260;
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 %d %d\" role=\"img\">\n<title>", width, height);
    html(f, title); fputs("</title><desc>Generated dependency graph; arrows show what each stage requires.</desc>\n", f);
    fputs("<defs><marker id=\"a\" markerWidth=\"8\" markerHeight=\"8\" refX=\"7\" refY=\"4\" orient=\"auto\">"
          "<path d=\"M0 0 8 4 0 8\" fill=\"none\" stroke=\"#94a5a2\"/></marker></defs>\n", f);
    for (unsigned i = 0; i < g->count; i++) {
        struct graph_node *n = &g->nodes[i];
        n->x = (int)((used[n->depth]++ + 1) * (unsigned)width / (counts[n->depth] + 1));
        n->y = n->depth * 112 + 24;
    }
    for (unsigned i = 0; i < g->edge_count; i++) {
        struct graph_node *a = &g->nodes[g->edges[i].from], *b = &g->nodes[g->edges[i].to];
        fprintf(f, "<path d=\"M%d %d C%d %d %d %d %d %d\" fill=\"none\" stroke=\"#566864\" marker-end=\"url(#a)\"/>\n",
            a->x, a->y + 64, a->x, a->y + 88, b->x, b->y - 24, b->x, b->y - 4);
    }
    for (unsigned i = 0; i < g->count; i++) {
        struct graph_node *n = &g->nodes[i];
        unsigned level_count = counts[n->depth];
        int box = level_count > 3 && width == 1000 ? 176 : 240;
        fprintf(f, "<g><rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"64\" rx=\"4\" fill=\"#192024\" stroke=\"%s\"/>"
            "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" fill=\"#dce5e3\" font-family=\"monospace\" font-size=\"12\">",
            n->x - box / 2, n->y, box, color(n->state), n->x, n->y + 25);
        html(f, n->label);
        fprintf(f, "</text><text x=\"%d\" y=\"%d\" text-anchor=\"middle\" fill=\"%s\" font-family=\"monospace\" font-size=\"10\">",
                n->x, n->y + 46, color(n->state));
        html(f, n->state); fputs("</text></g>\n", f);
    }
    fputs("</svg>\n", f);
}

int graph_generate(const char *path, const char *svg, const char *title)
{
    struct graph graph;
    if (graph_load(&graph, path) || make_parents(svg)) return 1;
    FILE *f = fopen(svg, "w");
    if (!f) return 1;
    graph_draw(f, &graph, title);
    int failed = ferror(f); failed |= fclose(f) != 0;
    return failed;
}
