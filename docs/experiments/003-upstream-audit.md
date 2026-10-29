# 003 / pinned upstream kernel / 2026-10-29

Hypothesis: the current upstream ARM64 kernel can be fetched at an exact reviewed
commit and cross-built in the remote lab independently of old project code.

Falsifier: the fetched commit differs, the minimal ARM64 image fails to build,
or the build consumes local PC resources.

Change: `./build kernel-audit` fetches v7.3 at
`c03c1a14507dfaf8b856c2608f9ac216021fc222` and builds tinyconfig in Codespaces.
The upstream annotated tag was resolved through GitHub's repository API.

This is a compiler/source baseline, not a P80h kernel configuration or a flashable
image. It proves no hardware support. No tablet boot experiment is authorized by
passing this build alone; board specifications and independent recovery come first.

Result: pending remote cross-build. Next: inventory upstream MT8163 support and
document each missing controller independently of historical vendor-derived code.
