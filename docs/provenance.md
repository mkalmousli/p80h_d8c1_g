# Provenance and source policy

| Component | Origin | License / treatment |
|---|---|---|
| Authored C, docs, site | New implementation in this repository | MIT |
| SVG artwork | Original hand-authored vectors | MIT |
| Existing tablet daemon | Prior installation, not imported | Transitional external protocol |
| Old driver findings | Historical research leads | Must be independently verified |
| ROM query facts | FOSS mtkclient protocol reference | Independent C implementation; no copied vendor code |
| Linux / Alpine | Not fetched or packaged yet | Preserve upstream licenses |
| Build environment | Codespaces Ubuntu image / Actions Ubuntu runner | External tooling |
| Device libc | Pinned musl 1.2.5 source archive | Static; upstream copyright and complete libc source bundled |

This is fresh code plus audited upstream FOSS, not formally separated clean-room
development. Device behavior can inform specifications. Never copy decompiled
vendor code or import a binary because it happens to work.

## Release boundary

No proprietary firmware, device dumps, stock images, keys, identifiers or calibration.
The source allowlist and packaging allowlist reduce accidental inclusion; they do
not replace review. Public build inputs must be generic, reproducible specifications.
Keep private inputs in ignored `local/`, never in Codespaces or CI by default.

The host binary links the runner's glibc; no host libc is bundled. Each release records compiler
version and source commit. Byte-for-byte reproducibility and signed image loading
are future milestones, not claims of the initial host-tools package.

Device releases cross-build musl from its pinned public source instead of bundling
the build host's libc. The archive and its upstream copyright travel with the
server binary. Compiler runtime support retains the GCC Runtime Library Exception.

## Upstream references

- [Linux source and releases](https://www.kernel.org/)
- [ARM64 boot contract](https://docs.kernel.org/arch/arm64/booting.html)
- [Android boot image format](https://android.googlesource.com/platform/system/tools/mkbootimg/)
- [Alpine](https://alpinelinux.org/)
- [GitHub Codespaces lifecycle](https://docs.github.com/en/codespaces/about-codespaces/understanding-the-codespace-lifecycle)
