/* SPDX-License-Identifier: MIT */
#include "web.h"

int maps_generate(void)
{
    if (graph_generate("data/boot-flow.tsv", "out/site/assets/boot-flow.svg", "Native boot architecture") ||
        graph_generate("data/development-flow.tsv", "out/site/assets/development-flow.svg", "Remote experiment loop") ||
        feature_graph()) return 1;
    FILE *out = fopen("out/site/diagrams.html", "w");
    if (!out) return 1;
    page_head(out, "How the pieces fit");
    fputs("<p class=eyebrow>GENERATED SYSTEM MAPS</p><h1>How the pieces fit.</h1>"
        "<p>Arrows show the order and dependencies. Color and text describe evidence. "
        "These SVGs are regenerated from the same versioned data as the project.</p>"
        "<h2>Power to operating system</h2><p>The processor starts inside a physical mask ROM. "
        "Our target is free first-stage code, DRAM initialization, monitor and native OS selection. "
        "No stock files may be required by the finished installer. Replacing the physical ROM "
        "itself requires different silicon.</p>"
        "<img class=system-map src=assets/boot-flow.svg alt=\"Boot stages and OS choices\">"
        "<p><a href=files/data/boot-flow.tsv.html>Inspect graph data</a></p>"
        "<h2>Source to measured result</h2><p>Independent remote workers build and test. "
        "The host transfers artifacts and owns USB. Tablet experiments run one at a time; "
        "sanitized results update documentation and this site.</p>"
        "<img class=system-map src=assets/development-flow.svg alt=\"Remote build and tablet evidence loop\">"
        "<p><a href=files/data/development-flow.tsv.html>Inspect graph data</a></p>"
        "<h2>What blocks what?</h2><p>Each feature record names its next prerequisite. "
        "This map is derived directly from the driver inventory. "
        "Open the SVG to zoom into the complete diagram.</p>"
        "<a href=assets/hardware-dependencies.svg><img class=system-map "
        "src=assets/hardware-dependencies.svg alt=\"Hardware dependencies and support states\"></a>"
        "<p><a href=hardware.html>Read individual driver records</a></p>", out);
    page_foot(out);
    int failed = ferror(out); failed |= fclose(out) != 0;
    return failed;
}
