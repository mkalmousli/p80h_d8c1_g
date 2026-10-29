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

Two workers: platform builds and independent validation. Each uses its own branch
and build directory. Artifacts travel through Codespaces copy or Actions downloads.
Verify SHA256SUMS before running host tools. The PC performs no source compilation.

```console
mp mk-gh codespace cp -c NAME remote:/workspaces/p80h_d8c1_g/out/p80h local/bin/p80h
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
| `p80h ping/status/sh/get` | Existing daemon transport, rewritten in C |

`data/progress.tsv` uses five pipe-separated fields; the filename denotes tabular
data, not literal tab delimiters. `out/progress.md` and site tables share this source.
Generated files are ignored. All tracked authored files have at most 100 lines.
