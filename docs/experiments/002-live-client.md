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

Result: verified with client source be4e205. Wallpaper SHA256 after upload matches
the remotely rendered file. Applied through existing feh; screenshot retrieved in
C and reviewed for private content. Public capture metadata: data/captures.tsv.

Critical current boot regions backed up privately through bounded C range GET:
boot0, boot1, LK, boot, recovery and primary GPT. Local hashes match device readback.
This is not a full filesystem backup or a demonstrated early-firmware restore.
