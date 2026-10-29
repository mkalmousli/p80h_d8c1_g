# Bring-up record

## 2026-10-29 / 001 / repository foundation

Hypothesis: a C-only host foundation can build independently in Codespaces, with
one structured progress record driving both documentation and the static site.

Falsifier: remote strict builds/tests fail, authored files exceed 100 lines, site
generation diverges from the progress record, or private inputs enter artifacts.

Change: new independent repository; no old source or device images imported.

Verification: Actions and both Codespaces pass strict C builds, parser/menu and
daemon framing tests. Generated library, feature pages and dependency SVGs pass
data and Markdown safety tests. Run 002 verifies C transport on the tablet.

Hardware: existing CLI reaches the installed daemon; live aarch64 and MT8163 model
observed. Existing kernel reports 6.12.99-p80h-dirty. No partition writes performed.
Owner cancelled stock restoration: preserve current installation and continue.
Fresh backups and independent recovery are required before boot-stage changes.

## Required experiment record

Date / run / hypothesis / falsifier / one change / source commit / artifact SHA256 /
device observation / result / rollback. Sanitized results only; raw logs stay local.

## 2026-10-29T19:08:38Z / 004 / binary link migration

Hypothesis: a fresh static ARM64 server can speak P80H/1 over USB alongside the
existing daemon, with no startup or boot partition changes.

Falsifier: negotiation/status fail, shell exit codes differ, binary downloads or
multi-chunk upload hashes differ, or the existing connection becomes unavailable.

Change: stage the cross-built server under /tmp and run it on 172.16.42.1:4445.
Rollback: stop that recorded process; the original daemon on port 4444 remains.

Preflight: both Codespaces pass strict builds and P80H/1 host integration tests.
Result: pending live USB acceptance; do not remove the original daemon yet.

Live: binary ping/status/shell exit 7 pass. A 3 MiB random-file round trip matches
SHA256 at source, on-device and after download. Boot0 download fails; the old
daemon remains usable. Hypothesis: the experimental USB path cannot sustain the
maximum frame burst. Falsifier: the same failure remains with 64 KiB chunks.
Change: bound default client chunks to 64 KiB; keep the 1 MiB protocol ceiling.

Repeat: boot0 and boot1 (4 MiB each) now match the earlier verified hashes, and
the 3 MiB fixture still round-trips correctly. The burst hypothesis is supported;
the precise kernel/USB failure mechanism is not yet established.

## 2026-10-29T19:30:58Z / 005 / native ROM information probe

Hypothesis: the new usbfs C probe can synchronize with MT8163 during a controlled
reboot window, query hardware/security state and return to the existing installation.
Falsifier: no handshake, incorrect query decoding, unexpected state change, or no
return of the old daemon after a physical power reset.

Change: run the bounded probe while requesting an orderly reboot. No flash/memory
writes are implemented in this probe. Owner is present for physical button recovery.
Preflight: synthetic descriptor/query tests pass; both current boot-region copies
match the verified private backups. Result: pending live ROM/preloader observation.

Warm reboot: existing daemon returned on the same installed kernel; no successful
ROM session was observed. Owner-assisted physical entry is the next bounded test.

## 2026-10-29T19:55:00Z / 006 / bounded preloader watchdog reset

Hypothesis: after the identity-free MT8163 query, a C-only reset command using
the documented MT8163 watchdog registers will leave preloader and return Linux.
Falsifier: bad echo/status, protected-memory flags, no USB reset request, or the
existing daemon fails to return within 60 seconds.

Change: allow only watchdog MODE/SWRST accesses; no caller-supplied addresses,
flash commands, identity queries or persistent writes. Owner is ready to use the
existing button menu as a physical fallback. Recovery is verified only when the
original daemon responds again.

## 2026-10-29T19:28:18Z / 003 / upstream audit result

Official kernel.org archive hash matched its published checksum. ARM64 tinyconfig
build passed remotely. The generic 1.9 MiB Image was not flashed. No mt8163 source
matches were found in the scanned DTS/binding/driver directories. The new kernel
therefore remains a toolchain baseline, not accepted P80h hardware support.
