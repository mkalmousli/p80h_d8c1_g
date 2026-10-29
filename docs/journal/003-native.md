# 003 / Preparing native boot

Recorded: 2026-10-29T18:49:32Z. Validation checkpoint: c6afb89.

Before attempting a native handoff, we needed contracts that could fail safely.
The new C validators reject malformed ARM64 headers, overflowing regions,
overlapping payloads and reserved-memory conflicts. Synthetic tests pass remotely.

They do not initialize DRAM, discover trustworthy memory, authenticate images,
parse a full DTB or enter a kernel. Those boundaries matter more than a green test.

The upstream audit pinned v7.3 to commit `c03c1a14507dfaf8b856c2608f9ac216021fc222`.
The official archive hash matched kernel.org's published SHA256, and ARM64
tinyconfig built remotely. Its 1.9 MiB Image is only a toolchain baseline. The
source scan found no MT8163 DTS, binding or driver matches in the audited paths.

On 2026-10-29 the identity-free C usbfs probe synchronized during a controlled
reboot and reported MT8163, subcode `8a00`, SBC/SLA/DAA clear. A separate bounded
watchdog reset was then sent after the fresh query. USB briefly enumerated as
`0e8d:0003`; the original Linux daemon returned. This verifies reset-and-return
from the observed preloader path, not erased-device recovery or a new boot chain.

The factory ROM is fabricated inside the processor. Our stock-free target covers
all writable stages and required loadable firmware; replacing that physical ROM
would require different silicon. No private donor file may be an installer input.
