# P80h source rules

- Authored implementation is C; every code file is at most 100 lines.
- `build.c` is the sole project build entry; upstream kernel/libc keep their own systems.
- `build.c` builds the host tools and runs the project checks.
- Keep device identity, stock files, calibration, raw dumps and logs in ignored `local/`.
- Before hardware changes, record the hypothesis and falsifier in the README.
- Change one hardware variable per experiment; keep partition writes behind the README write gate.
- Credit upstream sources in code comments and retain their licenses in packages.
- Commit and push verified work. Hardware claims need live evidence.
- Do not track device screenshots or private device data.
