/* SPDX-License-Identifier: MIT */
#include "feature-map.h"
#include <string.h>
struct map_context { struct inventory *data; FILE *out; unsigned seen[64]; };

static int lookup(struct inventory *s, const char *id)
{
    for (unsigned i = 0; i < s->graph.count; i++) if (!strcmp(id, s->graph.nodes[i].id)) return (int)i;
    return -1;
}

static void node_link(FILE *out, const struct graph_node *n)
{
    fputs("<a href=\"hardware/", out); html(out, n->id); fputs(".html\">", out);
    html(out, n->label); fputs("</a> <span class=state data-state=", out);
    html(out, n->state); fputs(">", out); html(out, n->state); fputs("</span>", out);
}

/* Vertical chains and disclosure cards remain readable without horizontal zoom. */
static void card(struct map_context *c, unsigned i)
{
    struct inventory *s = c->data;
    FILE *out = c->out;
    struct graph_node *n = &s->graph.nodes[i];
    fputs("<details class=dependency-card><summary><span>", out); html(out, n->label);
    fputs("</span><span class=state data-state=", out); html(out, n->state);
    fputs(">", out); html(out, n->state); fputs("</span></summary><div class=dependency-body>", out);
    unsigned chain[64], count = 0;
    int at = lookup(s, s->dependency[i]);
    while (at >= 0 && count < 64) { chain[count++] = (unsigned)at; at = lookup(s, s->dependency[at]); }
    fputs("<h3>What it needs</h3>", out);
    if (!count) fputs("<p>No earlier project feature is recorded as a prerequisite.</p>", out);
    else {
        fputs("<ol class=dependency-chain>", out);
        while (count) { fputs("<li>", out); node_link(out, &s->graph.nodes[chain[--count]]); fputs("</li>", out); }
        fputs("</ol>", out);
    }
    fputs("<h3>What it enables next</h3><ul>", out);
    unsigned dependents = 0;
    for (unsigned j = 0; j < s->graph.count; j++) if (!strcmp(s->dependency[j], n->id)) {
        fputs("<li>", out); node_link(out, &s->graph.nodes[j]); fputs("</li>", out); dependents++;
    }
    if (!dependents) fputs("<li>No immediate dependent is recorded.</li>", out);
    fputs("</ul><p>", out); node_link(out, n);
    fputs(" — open the driver record for evidence and the next test.</p></div></details>", out);
}

static int group(char **v, size_t columns, void *context)
{
    (void)columns;
    struct map_context *c = context;
    fputs("<section class=dependency-group><h2>", c->out); html(c->out, v[1]);
    fputs("</h2><div class=dependency-cards>", c->out);
    char *id = v[2];
    while (id) {
        char *next = strchr(id, ','); if (next) *next++ = 0;
        int i = lookup(c->data, id);
        if (i < 0 || c->seen[i]++) return 1;
        card(c, (unsigned)i); id = next;
    }
    fputs("</div></section>", c->out); return ferror(c->out);
}

int feature_map(struct inventory *data)
{
    FILE *out = fopen("out/site/hardware-map.html", "w");
    if (!out) return 1;
    struct map_context c = {.data = data, .out = out, .seen = {0}};
    page_head(out, "What blocks what");
    fputs("<p class=eyebrow>DEPENDENCIES / ONE FEATURE AT A TIME</p><h1>What blocks what?</h1>"
          "<p>Choose a subsystem, then expand a feature. Read its prerequisite chain "
          "from top to bottom and see what it enables next. Status describes evidence, "
          "not an estimate of remaining effort.</p>", out);
    int failed = table_read("data/hardware-groups.tsv", 3, group, &c);
    for (unsigned i = 0; i < data->graph.count; i++) if (!c.seen[i]) failed = 1;
    page_foot(out); failed |= ferror(out); failed |= fclose(out) != 0; return failed;
}
