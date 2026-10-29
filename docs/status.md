# Current status · 2026-10-29

- The installed MT8163 Linux system is preserved; no boot partition was changed.
- C ROM query read MT8163/security state. One restricted watchdog reset returned
  the existing Linux daemon; this is not independent recovery.
- P80H/1 C host and static ARM64 server work over USB. Serial 64 KiB reads are
  accepted; boot0 matches its earlier verified private copy.
- The C driver inventory is available as `p80h features`. Linux 7.3 tinyconfig
  builds, but the audited tree has no exact MT8163 DTS or driver support.
- Private boot/GPT copies are verified. A raw 7.75 GiB user-area read is still
  running under ignored `local/backups/`; CRCs are checked per frame. The mounted
  filesystem makes it an incoherent reference, not a restore proof. Never upload it.
- A four-request GET window stalled USB service and was removed. Keep reads serial.
- No new bootloader, kernel, native OS install or flashable firmware is complete.

Next: finish and size-check the private read; establish independent recovery and
board-level Linux support before any persistent boot change.
