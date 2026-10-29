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

Result: GitHub clone stalled and archive fallback was rate-limited (HTTP 429).
Switched to the official kernel.org release archive, pinned SHA256
87363c24f10e06321eb362765521094e94471f6bff8bce2d6b3e40f39f004953.
The downloaded hash matches the published kernel.org checksum. PGP signature
verification is not claimed by this initial audit.

ARM64 tinyconfig cross-build passed in Codespaces. Image SHA256:
10bdcc32236f18c7370e01b1fd44cea72099e66714a695a890444de08693af59.
The image is about 1.9 MiB and was not flashed. Build identity uses generic p80h /
remote strings, not device identity. A source scan found no mt8163 matches in
ARM64 DTS, devicetree bindings or drivers: board/controller work remains necessary.
