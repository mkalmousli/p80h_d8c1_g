# The generated project reference

The site is a build artifact, not a second manually maintained project.
Run `./build site` remotely. Pages deploys it automatically after main updates.

| Repository source | Generated result |
|---|---|
| `data/project.tsv` | Identity, summary and project URL base |
| `data/progress.tsv` | Milestones and generated progress Markdown |
| `data/features.tsv` | Every hardware/driver detail page and dependency graph |
| `data/boot-flow.tsv` | Native boot-stage SVG |
| `data/development-flow.tsv` | Remote experiment-loop SVG |
| `data/captures.tsv` | Reviewed screenshot gallery with run and source revision |
| Every tracked text file | Rendered docs or escaped source/data library |
| `site/index.html` | Presentation template; explicit tokens supply repo data |

## How generation works

1. Validate source policy and structured data.
2. Escape dynamic HTML and resolve relative documentation links.
3. Generate driver pages and the tracked-file library.
4. Resolve dependency IDs; reject missing nodes and cycles.
5. Lay out SVG maps deterministically, without browser JavaScript.
6. Check every generated internal link and image source.
7. Upload only `out/site/` to Pages.

The Markdown renderer supports headings, links, inline code, lists, tables and
fenced code. Raw HTML is displayed as text. Unsupported syntax remains readable.
Mermaid source is shown as code; the Maps page provides generated SVG alternatives.

## Evidence states

- Planned: a requirement without accepted implementation evidence.
- Observed: a binding or measurement exists; function is not proven.
- Implemented: code exists and its limits are stated.
- Verified: the precise behavior in the evidence field passed its test.
- Blocked: a named dependency prevents acceptance.

Text accompanies colors. Driver binding alone never qualifies as working hardware.
Reviewed PNG captures are the only binary source exception, confined to the gallery.
