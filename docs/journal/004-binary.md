# 004 / A protocol of our own

Recorded: 2026-10-29T18:59:02Z. State: implementation and testing in progress.

The old daemon's line-based transport got us access, but it mixed framing,
commands, text trailers and JSON status. The owner asked for a custom binary link.

P80H/1 uses an explicit 24-byte header, a versioned handshake, request IDs, typed
errors and bounded payloads. Status is a fixed binary record. File transfers are
chunked; uploads become visible only when the final chunk commits.

The new server binds a separate USB address/port. The old connection stays alive
until host integration tests, a remote static ARM64 build and live USB checks pass.
This is a migration strategy, not an excuse to claim compatibility without tests.

Read [the protocol contract](../protocol.md) for wire offsets and failure behavior.

## 2026-10-29T20:11Z / Safe resume over a serial window

A four-request read window passed the mock round trip and matched a 4 MiB
boot-region backup. Sustained USB traffic then stalled both temporary and legacy
services. Closing that client restored them, but the speed hypothesis failed.
The committed client keeps one GET in flight and can explicitly resume an
existing partial file. Remote checks pass; a serial 4 MiB boot0 read matches the
previous backup. The live user-area image remains a private, incomplete read.
