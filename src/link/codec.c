/* SPDX-License-Identifier: MIT */
#include "wire.h"
#include <string.h>

/* Explicit network-byte-order encoding works on either host architecture. */
uint16_t wire_u16(const unsigned char *p) { return (uint16_t)((unsigned)p[0] << 8 | p[1]); }
uint32_t wire_u32(const unsigned char *p) { return (uint32_t)wire_u16(p) << 16 | wire_u16(p + 2); }
uint64_t wire_u64(const unsigned char *p) { return (uint64_t)wire_u32(p) << 32 | wire_u32(p + 4); }
void wire_put16(unsigned char *p, uint16_t v) { p[0] = (unsigned char)(v >> 8); p[1] = (unsigned char)v; }
void wire_put32(unsigned char *p, uint32_t v) { wire_put16(p, (uint16_t)(v >> 16)); wire_put16(p + 2, (uint16_t)v); }
void wire_put64(unsigned char *p, uint64_t v) { wire_put32(p, (uint32_t)(v >> 32)); wire_put32(p + 4, (uint32_t)v); }

uint32_t wire_crc(const unsigned char *p, size_t n)
{
    uint32_t crc = UINT32_MAX;
    for (size_t i = 0; i < n; i++) {
        crc ^= p[i];
        for (unsigned bit = 0; bit < 8; bit++)
            crc = (crc >> 1) ^ (0xedb88320U & (0U - (crc & 1U)));
    }
    return ~crc;
}

int wire_header(unsigned char *h, const struct wire_message *m)
{
    if (!m || !m->id || !m->op || m->flags > WIRE_REPLY || m->size > WIRE_LIMIT ||
        (m->size && !m->body)) return 1;
    memcpy(h, "P80H", 4); h[4] = 1; h[5] = 0;
    wire_put16(h + 6, m->op); wire_put16(h + 8, m->flags); wire_put16(h + 10, m->status);
    wire_put32(h + 12, m->id); wire_put32(h + 16, m->size);
    wire_put32(h + 20, wire_crc(m->body, m->size));
    return 0;
}

/* Unsupported major/minor versions fail before allocation or command dispatch. */
int wire_decode(const unsigned char *h, struct wire_message *m)
{
    *m = (struct wire_message){0};
    if (memcmp(h, "P80H", 4) || h[4] != 1 || h[5] != 0) return 1;
    m->op = wire_u16(h + 6); m->flags = wire_u16(h + 8); m->status = wire_u16(h + 10);
    m->id = wire_u32(h + 12); m->size = wire_u32(h + 16);
    return !m->op || !m->id || m->flags > WIRE_REPLY || m->size > WIRE_LIMIT;
}
