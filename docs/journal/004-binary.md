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
