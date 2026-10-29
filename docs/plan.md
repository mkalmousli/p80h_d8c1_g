# Roadmap

This is a hardware bring-up plan, not a promise that every block already works.
Progress evidence is in [status](status.md), dated [journal](journal/001-foundation.md)
and [experiments](experiments/004-live-tree.md). No persistent boot write is allowed
until the recovery gate below is met.

## Milestones

| Gate | State | Work and acceptance |
|---|---|---|
| 0 · Preserve | In progress | Keep the installed OS. Validate GPT and private boot copies; finish a private user-area read. Record whether the live image is coherent. |
| 1 · C access | Working, temporary | C usbfs probe, restricted watchdog return, P80H/1 live link and verified boot-region reads. Relaunch the RAM agent after reboot; test full-transfer resume. |
| 2 · Board facts | In progress | Compare the sanitized runtime tree, peripherals, register maps and binding schemas with pinned upstream sources. Resolve charger identity and firmware provenance. |
| 3 · Open platform | Blocked on evidence | Implement and validate MT8163 DTS, clocks, pin control, storage, power, display/input and required FOSS firmware. Each controller needs cold-boot, error and resume acceptance. |
| 4 · Early stages | Not started | Specify reset state, DRAM, exception levels, memory reservations and monitor interfaces. Build an auditable writable first stage and recovery image. The physical mask ROM remains silicon code. |
| 5 · Native menu | Foundations only | Add board display/input, storage, settings and direct ARM64 Image+DTB+initrd handoff. No kexec. Test cancellation and failure fallback in RAM first. |
| 6 · Alpine | Not started | Build a pinned mainline kernel with accepted P80h board support and boot plain Alpine by default. Preserve a local recovery entry and verify repeated cold starts. |
| 7 · Peripherals | Partial inventory | Validate touch, keys, audio, radios, camera, power and suspend individually. Do not mark a bound driver working without its acceptance evidence. |
| 8 · Release | Not started | Publish only reviewed, reproducible FOSS tools/firmware with source, licenses, provenance, checksums and no device-specific identifiers or dumps. |

## Persistent-write gate

Before changing any boot or storage partition, require all of the following:

1. A complete private backup with exact geometry and independently checked contents.
2. A tested restore path that does not rely on the partition being replaced.
3. A RAM-only candidate, checksum, explicit hypothesis and falsifying test.
4. A written rollback procedure and device-owner readiness for the experiment.

The watchdog return test only recovered through the existing installed boot chain.
It does not satisfy independent recovery. A live eMMC copy can change while mounted
and cannot be called a coherent snapshot or restore proof.

## Build and review rules

Use `build.c` as the sole project build entry. Compile and test in Codespaces or
Actions; do not build on the host. Keep authored code files within 100 lines,
record provenance, and push each verified step with its evidence. Avoid website
redesign while platform bring-up is active; generated site content follows repo data.
