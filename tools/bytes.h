/* SPDX-License-Identifier: MIT */
#ifndef P80H_BYTES_H
#define P80H_BYTES_H
#include <stdint.h>
/* Bytewise decoding avoids unaligned accesses and host-endian assumptions. */
static inline uint32_t read_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static inline uint64_t read_le64(const unsigned char *p)
{
    return read_le32(p) | (uint64_t)read_le32(p + 4) << 32;
}
#endif
