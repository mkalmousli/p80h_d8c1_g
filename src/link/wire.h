/* SPDX-License-Identifier: MIT */
#ifndef P80H_WIRE_H
#define P80H_WIRE_H
#include <stddef.h>
#include <stdint.h>
#define WIRE_HEADER 24
#define WIRE_LIMIT (1024U * 1024U)
#define WIRE_CHUNK (64U * 1024U)
#define WIRE_REPLY 1
enum wire_op { WIRE_HELLO = 1, WIRE_PING, WIRE_STATUS, WIRE_EXEC, WIRE_GET, WIRE_PUT };
enum wire_error { WIRE_OK, WIRE_INVALID, WIRE_UNSUPPORTED, WIRE_IO, WIRE_TIMEOUT,
                  WIRE_TOO_LARGE, WIRE_STATE };
struct wire_message {
    uint16_t op, flags, status;
    uint32_t id, size;
    unsigned char *body;
};
uint16_t wire_u16(const unsigned char *);
uint32_t wire_u32(const unsigned char *);
uint64_t wire_u64(const unsigned char *);
void wire_put16(unsigned char *, uint16_t);
void wire_put32(unsigned char *, uint32_t);
void wire_put64(unsigned char *, uint64_t);
uint32_t wire_crc(const unsigned char *, size_t);
int wire_header(unsigned char *, const struct wire_message *);
int wire_decode(const unsigned char *, struct wire_message *);
/* recv allocates body; caller frees it. No host structs appear on the wire. */
int wire_send(int, const struct wire_message *);
int wire_recv(int, struct wire_message *);
#endif
