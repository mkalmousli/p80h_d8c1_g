# 003 / Preparing native boot

Recorded: 2026-10-29T18:49:32Z. Validation checkpoint: c6afb89.

Before attempting a native handoff, we needed contracts that could fail safely.
The new C validators reject malformed ARM64 headers, overflowing regions,
overlapping payloads and reserved-memory conflicts. Synthetic tests pass remotely.

They do not initialize DRAM, discover trustworthy memory, authenticate images,
parse a full DTB or enter a kernel. Those boundaries matter more than a green test.

The upstream audit pinned v7.3 to an exact commit. The GitHub clone stalled and
the archive fallback returned HTTP 429. No successful new kernel build is claimed.
The next experiment must resolve source retrieval before comparing controller
support and preparing a board-specific kernel configuration.

The factory ROM is fabricated inside the processor. Our stock-free target covers
all writable stages and required loadable firmware; replacing that physical ROM
would require different silicon. No private donor file may be an installer input.
