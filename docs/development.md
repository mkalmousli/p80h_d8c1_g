# Remote development

Do not build or run automated tests on the host PC.

```console
mp mk-gh codespace list
mp mk-gh codespace ssh -c NAME
```

Inside a Codespace:

```console
git pull --ff-only
cc -std=c11 -Wall -Wextra -Werror -pedantic build.c -o build
./build test
./build package
```

Use separate Codespaces for parallel platform builds and independent validation.
Check out the same committed source in separate workspaces; keep their `out/`
directories isolated. Tablet experiments are serialized even when builds run in
parallel. In this environment `codespace cp` did not transfer existing files, so
stream public build artifacts over SSH and compare both SHA256 values:

```console
mp mk-gh codespace ssh -c NAME -- 'cat /workspaces/p80h_d8c1_g/out/p80h' > local/bin/p80h
sha256sum local/bin/p80h
mp mk-gh codespace ssh -c NAME -- 'sha256sum /workspaces/p80h_d8c1_g/out/p80h'
```

Only identity-free executables may be transferred this way. Raw dumps, logs,
calibration, credentials and device-specific data stay in ignored local storage.
The PC performs no source compilation or automated tests.

```console
mp mk-gh codespace stop -c NAME
```

Stop workers after checkpointing. Start within existing quota and spending limits;
do not raise limits. Physical tablet tests remain serialized regardless of workers.

## Commands and interfaces

| Entry | Behavior |
|---|---|
| `./build tools` | Compile host tools and check source policy |
| `./build test` | Also verify menu/image behavior and generate site |
| `./build site` | Same verification, emit static site and progress Markdown |
| `./build package` | Also package host executable, source, licenses and hashes |
| `p80h usb` | List USB candidates without serials; not hardware identification |
| `p80h inspect IMAGE` | Validate Android v0 structure; no authenticity claim |
| `p80h ping/status/sh/get/resume/put` | P80H/1 binary USB link |
| `p80h legacy COMMAND` | Explicit transitional connection to the installed daemon |
| `p80h rom-info [--wait]` | Identity-free MT8163 ROM/preloader query |
| `p80h rom-reset [--wait]` | Restricted watchdog reset after fresh query and security gates |

`data/progress.tsv` uses five pipe-separated fields; the filename denotes tabular
data, not literal tab delimiters. `out/progress.md` and site tables share this source.
Generated files are ignored. All tracked authored files have at most 100 lines.
