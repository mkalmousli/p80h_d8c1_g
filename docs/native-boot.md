# Native OS handoff contract

The boot manager must load the OS directly. No kexec and no hidden Linux kernel
under the menu. The portable validation code is not yet a working bootloader.

## Implemented foundations

- ARM64 Image header validation, modern bounded little-endian images only.
- Checked extents, reserved-memory exclusions and non-overlapping payloads.
- DTB placement alignment and maximum size checks.
- Portable menu policy with an explicit default and cancelable timeout.

These functions perform no hardware I/O and never jump to an address. They are
host-tested building blocks. They do not validate signatures, parse a complete
device tree, discover safe RAM, initialize hardware or prove a handoff is safe.

## Platform work still required

1. Establish exception level, security state, DRAM and reserved-memory ownership.
2. Read filesystem/image inputs and authenticate them under the owner's policy.
3. Validate a complete DTB and apply only documented board-specific fixups.
4. Place the Image relative to a 2 MiB aligned base using its text_offset.
5. Respect initrd window constraints and every firmware memory reservation.
6. Quiesce DMA, prepare caches/timers/coherency and disable the MMU as required.
7. Enter the native kernel with x0 pointing to the DTB and x1 through x3 zero.
8. Demonstrate controlled failure and recovery before persistent installation.

Source contract: [Linux ARM64 boot documentation](https://docs.kernel.org/arch/arm64/booting.html).
Keep unsupported formats explicit. Never turn successful bounds checking into a
claim that an image is trusted, compatible, or ready to flash.
