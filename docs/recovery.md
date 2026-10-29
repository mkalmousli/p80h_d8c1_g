# Recovery and current-device continuity

The owner explicitly cancelled the stock reset on 2026-10-29. Continue from the
current installation. Do not erase user data or restore stock as an implicit step.

## Before any persistent boot/storage change

1. Establish the exact board, current boot chain and security state.
2. Record partition geometry and back up eMMC user and boot regions privately.
3. Hash backups and verify readback. Record where the tested restore tool lives.
4. Demonstrate recovery independently of the partition being changed.
5. Record hypothesis, falsifier, candidate hash and rollback procedure.
6. Change one variable and read back writes before rebooting.

Do not format all storage, overwrite calibration/identity, modify fuses, or assume
the historical watchdog fallback applies to a new boot chain. Never publish dumps.

## Current blockers

- Historical backup images are not a fresh complete backup.
- Recovery after replacement of early firmware has not been demonstrated.
- No flashable new firmware is provided by this project yet.

Read-only inspection and the existing daemon remain the first access path.
Continue using old tooling only for transport functions not yet replaced in C.
