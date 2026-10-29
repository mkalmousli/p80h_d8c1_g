# P80h

An open boot platform for the Teclast P80h (MT8163).
Native Alpine. Small C tools. Original artwork. Measured hardware support.

[Device & progress](https://mkalmousli.github.io/p80h_d8c1_g/) ·
[Roadmap](docs/plan.md) · [Specification](docs/specification.md) ·
[Architecture](docs/architecture.md) · [Recovery](docs/recovery.md) ·
[Remote development](docs/development.md) · [Bring-up log](docs/status.md)

**Early research. No flashable replacement firmware is available yet.**
The permanent silicon Boot ROM cannot be replaced. Writable stages and drivers
are research targets, not promises of existing support.

## Build remotely

In Codespaces or Actions only:

```console
cc -std=c11 -Wall -Wextra -Werror -pedantic build.c -o build
./build test
./build site
./build package
```

`build.c` is the entry point. There are no project Makefiles, shell scripts,
Python tools, or JavaScript dependencies. Upstream Linux retains its build tools.
All authored files are limited to 100 lines. Private data belongs in `local/`.

## Layout

| Directory | Purpose |
|---|---|
| `tools/` | C build, policy, site and host utilities |
| `src/boot/` | Portable boot-menu policy, independent of hardware |
| `tests/` | Host verification of parsers and boot policy |
| `data/` | Shared milestone and hardware records |
| `site/` | Static page templates, CSS and original SVG artwork |
| `docs/` | Contracts, experiments, recovery and provenance |

Existing vendor code and firmware are not imported. Dependencies and exceptions
are documented in [provenance](docs/provenance.md).
