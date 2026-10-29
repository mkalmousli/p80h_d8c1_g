# P80h platform specification

Status: evolving implementation contract, not a claim of complete hardware support.
Evidence lives in the timestamped journal and feature records. Unknowns remain
explicit. Public releases distinguish host tools, experimental payloads and accepted firmware.

## Intended result

Install plain Alpine and directly boot it through an owner-controlled, touch/key
boot manager. Support other Linux installations, locally supplied Android and
independent recovery. Do not use kexec or hide a Linux kernel underneath the menu.
Every writable stage and required loadable firmware must have an auditable FOSS
implementation. The finished installer requires no stock files or donor dumps.

The processor's physical mask ROM is not flash storage. A FOSS first-stage payload
and ROM-compatible transport can follow it; replacing the silicon ROM requires
different hardware. Never describe the current installation as fully free.

## Current evidence and problems

| Area | Implemented / observed | Unresolved problem |
|---|---|---|
| Host tooling | C transport, image/GPT inspection, private critical backups | Binary endpoint migration needs live acceptance |
| Boot manager | Tested portable policy and ARM64 bounds checks | No board renderer, filesystem loader or native handoff |
| Early boot | Current writable stages retained | DRAM, power and security interfaces lack complete free specs |
| Kernel | Existing device runs 6.12.99-p80h-dirty | New upstream audit fetch stalled / archive rate-limited |
| Factory evidence | Archived /proc/version: Linux 3.10.72, GCC 4.9.x | No archived stock 4.x kernel record found in checked logs |
| Storage | eMMC enumerates; primary GPT validates | Complete consistent backup and independent restore unproven |
| Battery | Charging / about 3.53 V / 0 percent reported | Telemetry accuracy and safe full charge cycle unverified |
| Radios / touch | Drivers bind in existing installation | Free required firmware provenance is not established |
| Graphics | Existing desktop screenshot captured | GPU acceleration and video support lack an accepted open path |

## Architectural contracts

- Host PC: USB, verification, transfers, private records; no builds or automated tests.
- Codespaces: isolated source/build trees, bounded concurrent workers, pinned inputs.
- Actions: independent verification, release artifacts and Pages deployment.
- Device transport: [P80H/1](protocol.md), bounded binary frames and typed responses.
- Native loader: [ARM64 contract](native-boot.md), no persistent install before recovery.
- Release boundary: [stock-free requirements](freedom.md), no identifiers or vendor blobs.
- Documentation: data-driven feature records, chronological journal and mobile dependency map.

## Required boot-manager behavior

Alpine is the default with a cancelable countdown. Touch and physical keys can
select entries, change boot order/timeout, open diagnostics and enter recovery.
Disabled entries explain their missing input. A failed boot must not permanently
hide the menu or destroy the independent recovery path. Settings need explicit
versioning and reset behavior before persistence is implemented.

Native Linux inputs are a kernel Image, board DTB, optional initrd and command line.
Android needs a separately verified loading/chainloading contract matching its
actual boot format. UEFI support is a later port, not promised by a menu label.

## Hardware acceptance

Each feature names a driver, evidence, prerequisite and next acceptance gate in
data/features.tsv. Binding alone is observed, not working. Acceptance requires
cold-boot and resume behavior, error handling and sustained operation appropriate
to that component. Firmware provenance is part of feature acceptance.

Storage needs read/write integrity and tested recovery; power needs measured safe
charging/voltage/thermal behavior; input needs all controls; display needs modes,
backlight and resume; radios need free firmware and traffic/pairing tests; media
needs real playback/capture. No feature becomes verified through a screenshot alone.

## Delivery gates

1. Remote C foundation and a verified device access path.
2. Complete private backup and recovery independent of the stage being replaced.
3. RAM-resident early-stage experiments with written hypotheses and falsifiers.
4. Free writable initialization and native menu/loader with controlled fallback.
5. Pinned upstream kernel with board-specific controller acceptance.
6. Stock-free Alpine installation tested without a useful Android installation.
7. Published source, licenses, checksums, provenance and exact support limitations.

Read [blockers](blockers.md) for the experiments needed to cross these gates.
