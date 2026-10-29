# Native C device agent

The new agent is a Linux userspace control channel, not a bootloader or monitor.
Its source is in src/link/. It binds the existing isolated USB network address,
speaks P80H/1, and supports status, shell operations and chunked file transfer.

## Verified installation so far

Server source checkpoint: 7b4a1e4; initial private live test used a static glibc
cross-build. Host checkpoint 0638e99 caps chunks at 64 KiB after a larger burst
failed on the experimental USB path. Both eMMC boot-region downloads then matched
their earlier verified hashes. A 3 MiB fixture passed upload/download SHA256 checks.

The server was staged under /tmp alongside the old daemon. It deliberately had no
startup changes and disappeared on reboot. Do not mistake that for a permanent
agent installation or independent erased-device recovery.

## Reproducible release build

Run ./build release in Codespaces/Actions with an ARM64 cross compiler. It runs
host tests, cross-builds a pinned musl libc, links a static ARM64 agent, and packages
host/server sources, libc source, copyright, revision and checksums. The package
contains no stock inputs, device calibration, credentials or per-device identifiers.

The musl build uses upstream configure/make under the agreed upstream exception;
all project orchestration stays in C. A new libc build must pass the same live USB
tests before replacing the accepted agent. There is no fallback to a different
static libc hidden inside the release target.

## Remaining work

- Verify musl-linked server on the tablet and persist it only with rollback inputs.
- Diagnose the precise large-burst USB/kernel failure; do not hide it as protocol success.
- Add authenticated sessions before any exposure beyond the isolated USB link.
- Design flash transactions and readback/recovery gates before exposing raw writes.
- Replace transitional ROM access with a hardware-verified C recovery path.
