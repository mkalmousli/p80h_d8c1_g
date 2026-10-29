# Working on P80h

- Authored tooling and firmware: C, at most 100 physical lines per file.
- Exceptions: minimal assembly, DTS, config, JSON, YAML, Markdown, HTML, CSS, SVG.
- Build entry point: `build.c`. Upstream projects retain their own build systems.
- Never compile, emulate, render screenshots, or run automated tests on the host PC.
  Use Codespaces through `mp mk-gh` and GitHub Actions; host is USB/transfer only.
- Independent remote builds may run concurrently; tablet writes must be serialized.
- Before hardware changes, record hypothesis and falsifier in `docs/status.md`.
- One hardware variable per experiment. Record source commit and artifact hash.
- Never flash without identified hardware, verified private backups, matching images,
  and a demonstrated recovery route. Never write unknown targets or burn fuses.
- Keep device identity, credentials, stock images, firmware and raw logs in ignored
  `local/`. Do not upload them. Public artifacts must be FOSS and identity-free.
- Hardware claims need evidence. Historical findings are leads, not verification.
- Document interfaces next to code. Prefer a small complete module over scaffolding.
- Commit and push each verified step. Never label host simulation hardware verified.
- Original SVG artwork only; palette and style are specified in `docs/design.md`.
