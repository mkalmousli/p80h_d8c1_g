# P80h

Open boot-platform research for the Teclast P80h (MT8163). The goal is direct
Alpine boot from a native C boot manager. There is no replacement firmware yet.

The host tool is C: `p80h features` lists the current driver evidence and next
acceptance gates; `p80h rom-info` queries the MediaTek boot path; `p80h` uses the
versioned P80H/1 USB link. The device agent is temporary in RAM.

The processor's mask ROM is fixed silicon code. The project can replace writable
stages after it, but cannot rewrite the physical ROM. No boot partition has been
changed by this project. See the [roadmap](docs/plan.md), [current status](docs/status.md)
and [remote build instructions](docs/development.md).

Build only in Codespaces or GitHub Actions:

```console
cc -std=c11 -Wall -Wextra -Werror -pedantic build.c -o build
./build test
./build package
```

Authored source is C and stays within 100 lines per file. Original vector artwork
is kept in `artwork/`. Raw dumps, credentials, calibration and device-specific
data stay in ignored `local/`; they never enter public artifacts.
