# Provenance and source policy

| Component | Origin | License / treatment |
|---|---|---|
| Authored C, docs, site | New implementation in this repository | MIT |
| SVG artwork | Original hand-authored vectors | MIT |
| Existing tablet daemon | Prior installation, not imported | Transitional external protocol |
| Old driver findings | Historical research leads | Must be independently verified |
| Linux / Alpine | Not fetched or packaged yet | Preserve upstream licenses |
| Build environment | Codespaces Ubuntu image / Actions Ubuntu runner | External tooling |

This is fresh code plus audited upstream FOSS, not formally separated clean-room
development. Device behavior can inform specifications. Never copy decompiled
vendor code or import a binary because it happens to work.

## Release boundary

No proprietary firmware, device dumps, stock images, keys, identifiers or calibration.
The source allowlist and packaging allowlist reduce accidental inclusion; they do
not replace review. Public build inputs must be generic, reproducible specifications.
Keep private inputs in ignored `local/`, never in Codespaces or CI by default.

The host binary currently links the runner's glibc. Each release records compiler
version and source commit. Byte-for-byte reproducibility and signed image loading
are future milestones, not claims of the initial host-tools package.

## Upstream references

- [Linux source and releases](https://www.kernel.org/)
- [ARM64 boot contract](https://docs.kernel.org/arch/arm64/booting.html)
- [Android boot image format](https://android.googlesource.com/platform/system/tools/mkbootimg/)
- [Alpine](https://alpinelinux.org/)
- [GitHub Codespaces lifecycle](https://docs.github.com/en/codespaces/about-codespaces/understanding-the-codespace-lifecycle)
