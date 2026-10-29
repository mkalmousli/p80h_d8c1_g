# Remote development

The host is for USB access and private transfers only. Do not build or run tests
locally. Use `mp mk-gh codespace list`, then work from a committed checkout:

```console
mp mk-gh codespace ssh -c NAME
git pull --ff-only
cc -std=c11 -Wall -Wextra -Werror -pedantic build.c -o build
./build test
```

Targets are `tools`, `test`, `device`, `package`, `release` and `kernel-audit`.
Actions runs the same source checks on pushes. Separate Codespaces may build in
parallel; tablet operations stay serialized.

Transfer only identity-free executables. Stream through SSH and compare hashes;
do not use Codespaces to copy raw device data:

```console
mp mk-gh codespace ssh -c NAME -- 'cat /workspaces/p80h_d8c1_g/out/p80h' > local/bin/p80h
sha256sum local/bin/p80h
mp mk-gh codespace ssh -c NAME -- 'sha256sum /workspaces/p80h_d8c1_g/out/p80h'
```

Keep dumps, logs, calibration and credentials in ignored `local/`. Stop idle
Codespaces after checkpointing. Device builds and all automated tests run remotely.
