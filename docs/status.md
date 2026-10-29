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
