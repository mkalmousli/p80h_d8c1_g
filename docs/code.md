# Reading the code

The project has three independent layers: host transport, portable boot policy,
and documentation generation. Hardware initialization and native kernel handoff
are not implemented yet. Do not confuse host-tested code with a working loader.

## Build orchestration

`build.c` checks the remote environment, compiles small C tools, runs meaningful
tests, generates the site, and packages release inputs. `tools/run.h` starts
subprocesses with argument arrays; it does not interpret a shell command string.

## Host transport

- `host.c`: command dispatch, numeric range parsing, privacy-limited image inspection.
- `net.c`: endpoint validation, connection timeout, complete sends and socket reads.
- `command.c`: bounded response handling and exact remote exit-status trailers.
- `get.c`: frame validation, streaming downloads, no overwrite, partial-file cleanup.
- `put.c`: uploads restricted to named temporary staging files, never device nodes.
- `image.c`: Android v0 extent validation; rejects unsupported extensions.
- `usb.c`: candidate discovery without exposing serials or claiming device identity.

The old tablet daemon remains the transitional server. The new client does not
import its Python host code. USB transport is not an authentication mechanism.

## Boot policy

`src/boot/menu.h` defines the public data/events contract. `menu.c` implements
selection, available-entry navigation and a cancelable countdown without I/O.
A future board layer must provide rendering, input, storage and native OS loading.

## Static reference generator

- `table.c`: strict bounded record parsing with synchronous callbacks.
- `html.c`: shared document shell, metadata and output escaping.
- `template.c`: known-token expansion; unknown tokens fail generation.
- `inline.c`, `markdown.c`: safe readable documentation rendering.
- `library.c`: indexes tracked text only; private directories are never crawled.
- `feature.c`, `summary.c`: driver detail pages, counts and device facts from data.
- `graph-data.c`, `feature-graph.c`: dependency validation and cycle detection.
- `graph-svg.c`: deterministic layered diagrams, independent of browser scripts.
- `gallery.c`: reviewed capture metadata and presentation.
- `links.c`: validates generated local URLs and images after generation.

Tests cover framing failures, binary transfer, parser bounds, overflow, unsafe
Markdown links and malformed dependency graphs. All verification runs remotely.
