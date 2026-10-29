# P80h source rules

- Authored implementation is C; every code file is at most 100 lines.
- `build.c` is the sole project build entry; upstream kernel/libc keep their own systems.
- Never build or run tests on the host. Use `mp mk-gh` Codespaces or Actions.
- Keep device identity, stock files, calibration, raw dumps and logs in ignored `local/`.
- Before hardware changes, record the hypothesis and falsifier in `docs/status.md`.
- Change one hardware variable per experiment; keep all partition writes behind the gate in `docs/plan.md`.
- Credit upstream sources in code comments and retain their licenses in packages.
- Commit and push verified work. Hardware claims need live evidence.
- Keep original vectors in `artwork/` and do not track device screenshots.
