# P80H/1 binary device link

Version: 1.0. Transport: reliable byte stream over isolated USB networking,
default device address 172.16.42.1, TCP port 4445. P80H_DAEMON is legacy-only;
P80H_LINK overrides the binary client's IPv4 endpoint. No JSON or line framing.

## Frame header (24 bytes, all integers big-endian)

| Offset | Bytes | Field | Contract |
|---|---|---|---|
| 0 | 4 | Magic | ASCII P80H |
| 4 | 1 | Major | 1; incompatible versions close the connection |
| 5 | 1 | Minor | 0; this implementation accepts exactly 1.0 |
| 6 | 2 | Opcode | Request type; zero invalid |
| 8 | 2 | Flags | 0 request, 1 response; other bits invalid |
| 10 | 2 | Status | Zero in requests; typed result in responses |
| 12 | 4 | Request ID | Nonzero, strictly increasing within a connection |
| 16 | 4 | Payload size | 0 through 1048576, checked before allocation |
| 20 | 4 | Payload CRC32 | IEEE CRC32; zero for an empty payload |

No C structure is copied onto the wire. A response repeats its request ID and
opcode. The receiver reads exact lengths despite arbitrary TCP fragmentation.
CRC detects framing/corruption mistakes; it is not authentication or encryption.

## Session and operations

HELLO must succeed before other operations. Reply payload is capabilities:u32
(0x3f for these six operations) and maximum payload:u32. A second HELLO is invalid.
At most 262144 requests per session; socket inactivity timeout is 35 seconds.

| Opcode | Name | Request | Successful response |
|---|---|---|---|
| 1 | HELLO | Empty | Capabilities:u32, payload limit:u32 |
| 2 | PING | Empty | Empty |
| 3 | STATUS | Empty | Fixed 32-byte record below |
| 4 | EXEC | 1–4096 command bytes, no NUL | Exit code:u32, raw stdout/stderr |
| 5 | GET | Offset:u64, limit:u32, absolute path bytes | Total size:u64, raw chunk |
| 6 | PUT | Path length:u16, path, offset:u64, total:u64, raw chunk | Empty acknowledgement |

EXEC runs an explicit privileged shell command; duration is limited to 30 seconds
and output to payload limit minus four bytes. Timeout/output-limit errors kill
its process group. Binary output and command newlines need no special escaping.

GET supports regular files and block devices. A zero limit queries size only.
The C client pipelines four ordered 64 KiB GETs and validates each reply ID,
length, total and CRC. `resume REMOTE LOCAL` continues an existing regular partial
file at its current length; ordinary `get` still refuses every existing path.
Size changes cause the client to fail; stable size does not make a mounted
filesystem a coherent snapshot.

PUT allows one in-flight upload per connection, only to /tmp/p80h-NAME without
subdirectories. Offset/total must remain consistent; total is at most 16 GiB.
The private staging file is committed by rename only after the last chunk and
fsync succeed. Normal disconnect/error cleans staging. Abrupt process/power loss
may leave an owner-only temporary file. No partition writes are exposed by PUT.

## Binary STATUS record

| Offset | Type | Meaning |
|---|---|---|
| 0 | u64 | Uptime in milliseconds |
| 8 | u64 | Available memory in bytes |
| 16 | u32 | Battery voltage in microvolts, UINT32_MAX if unavailable |
| 20 | u32 | Reported capacity percent, UINT32_MAX if unavailable |
| 24 | u32 | Availability flags: bit 0 voltage, bit 1 capacity |
| 28 | u32 | Reserved zero |

Availability does not certify measurement accuracy. Existing battery readings
are explicitly unverified. Additional status fields require a versioned contract.

## Errors, security and compatibility

0 OK; 1 invalid request; 2 unsupported operation; 3 I/O failure; 4 execution timeout;
5 output too large; 6 invalid session/upload state. Malformed frames, failed CRC,
unsupported versions or transport truncation close the connection before dispatch.

The server binds only the USB address, with at most four workers. This version
has no cryptographic peer authentication. Do not forward it to untrusted networks.
Authentication, flash transactions, interactive terminals and resumable uploads
across reconnects are not implemented. Existing text access is explicitly invoked
with p80h legacy; there is no silent downgrade from the binary client.
