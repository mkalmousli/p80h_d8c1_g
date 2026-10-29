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

## 2026-10-29T20:02:00Z / 007 / private eMMC read capacity

Hypothesis: a 256 Ki-request P80H/1 session can transfer the full 7.75 GiB
eMMC as 64 KiB reads into a private local backup with exact size and valid
per-frame CRCs. Falsifier: source size mismatch, incomplete transfer, invalid
frame CRC, or inability to keep the image private. No persistent device writes.

Change: raise the per-session request cap from 65536 to 262144, enough for
118289 GETs plus protocol setup. Validate source/build policy remotely first.
The live filesystem may change during a raw block read, so this is not a
coherent filesystem snapshot or restore proof. Raw data stays local; it is not
sent to Codespaces for hashing or analysis.
The partial transfer's first 34 sectors pass the C GPT CRC/bounds parser and
match the previously verified private primary GPT; the remaining image is pending.

## 2026-10-29T20:05:00Z / 008 / windowed binary reads

Hypothesis: four ordered GET requests in flight improve USB block-read speed at
least 2x while preserving each response ID, length, total size and CRC. A new
explicit resume command can continue a private partial file without overwriting
an existing destination. Falsifier: synthetic round-trip corruption, live boot
region mismatch, throughput below 2 MiB/s, or loss of the existing service.
Block reads are read-only; runtime service files stay in volatile storage.

Remote C tests passed. A 4 MiB pipelined boot-region read matched the existing
backup byte for byte. During sustained pipelined reads, transfer speed fell below
2 MiB/s and both USB services became briefly unavailable. Stopping the client
restored them; the partial local file was retained. The speed hypothesis is
rejected. Keep the window at one request.

The sequential client with explicit resume passed remote C checks. A fresh 4 MiB
boot0 read matched the earlier backup exactly in about four seconds. The 7.75 GiB
private user-area image resumed from its retained partial file; transfer remains
in progress and the mounted filesystem prevents a coherent snapshot claim.

## 2026-10-29T20:13:00Z / 009 / runtime SoC compatible inventory

The installed device tree declares `teclast,p80h` / `mediatek,mt8163` and 39
SoC child compatibles. The inventory is recorded without raw DTB or unique
properties. Exact MT8163 driver/binding matches were absent in the pinned 7.3
audit; shared-family strings remain candidates, not accepted support.
