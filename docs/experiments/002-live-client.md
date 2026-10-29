# 002 / C transport and original artwork / 2026-10-29

Hypothesis: the new C host client can replace Python for current daemon operations,
without altering boot partitions or the installed operating system.

Falsifier: ping/status disagree, command exit status is lost, downloads differ,
or staged uploads fail byte-for-byte readback verification.

Observed: C ping returns p80hd 0.2; status and shell work; `false` returns 1.
Live device: aarch64, MT8163 model, kernel 6.12.99-p80h-dirty. eMMC enumerates.
Screenshot captured through C GET; original framebuffer is 800 by 1280, 32 bpp.
Battery reports 0%, charging, voltage 3532512 microvolts; telemetry is not accepted
as working. Private logs and partition survey retained locally.

Artwork experiment hypothesis: a remotely rendered original SVG can be staged,
hash-verified and applied as a reversible wallpaper, producing a real screenshot.
Falsifier: upload hash differs, desktop fails to apply it, or screenshot differs
from the intended design. No startup configuration or boot partition changes.

Result: pending staged transfer verification and capture.
