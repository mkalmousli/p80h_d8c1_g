# Bring-up roadmap

## Current gates

1. **Preserve the installed system.** Critical boot copies and GPT are verified;
   a private raw user-area read is in progress. It is live, not a coherent snapshot.
2. **Finish board facts.** `p80h features` records observed drivers, gaps and
   acceptance checks. Exact MT8163 support is missing from the audited Linux 7.3 tree.
3. **Build open platform support.** Verify DTS, clocks, pin control, storage,
   power and required free firmware against the hardware.
4. **Replace writable boot stages.** Specify DRAM, reset, secure monitor and
   recovery behavior. The silicon mask ROM stays immutable.
5. **Boot natively.** Add display/input, storage and direct ARM64 kernel handoff;
   no kexec. Test failure and recovery from RAM first.
6. **Default to Alpine.** Require repeated cold boots and independent recovery.
7. **Release.** Publish source, licenses, checksums and identity-free artifacts only.

## Write gate

Do not alter boot or storage partitions until a complete private backup, verified
independent restore path, RAM-only candidate, explicit falsifier and rollback are
ready. The tested watchdog return used the installed boot chain and is not
independent recovery. See `src/device/features.c` for current board evidence and
acceptance gates, and C module comments for protocol, ROM and loader contracts.
