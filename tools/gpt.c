/* SPDX-License-Identifier: MIT */
#include "gpt.h"
#include "bytes.h"
#include <string.h>

uint32_t gpt_crc32(const unsigned char *data, size_t size)
{
    uint32_t crc = UINT32_MAX;
    for (size_t i = 0; i < size; i++) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; bit++)
            crc = (crc >> 1) ^ (0xedb88320U & (0U - (crc & 1U)));
    }
    return ~crc;
}

/* Validate both checksums and every extent before exposing any partition names. */
bool gpt_parse(const unsigned char *data, size_t size, struct partition_table *out)
{
    if (!out) return false;
    *out = (struct partition_table){0};
    if (!data || size < 1024 || memcmp(data + 512, "EFI PART", 8)) return false;
    const unsigned char *h = data + 512;
    uint32_t length = read_le32(h + 12), count = read_le32(h + 80), stride = read_le32(h + 84);
    if (read_le32(h + 8) != 0x10000 || length < 92 || length > 512 ||
        read_le32(h + 20) || !count || count > 128 || stride < 128 || stride > 512 || stride % 128)
        return false;
    unsigned char header[512];
    memcpy(header, h, length); memset(header + 16, 0, 4);
    if (gpt_crc32(header, length) != read_le32(h + 16) || read_le64(h + 24) != 1) return false;
    uint64_t first = read_le64(h + 40), last = read_le64(h + 48), entries = read_le64(h + 72);
    size_t bytes = (size_t)count * stride;
    if (entries < 2 || entries > size / 512 || first < 3 || last < first ||
        last >= read_le64(h + 32) || bytes > size - (size_t)entries * 512 ||
        entries >= first || (bytes + 511) / 512 > first - entries) return false;
    const unsigned char *table = data + (size_t)entries * 512;
    if (gpt_crc32(table, bytes) != read_le32(h + 88)) return false;
    struct partition_table result = {0};
    for (unsigned i = 0; i < count; i++) {
        const unsigned char *entry = table + i * stride;
        unsigned used = 0;
        for (unsigned j = 0; j < 16; j++) used |= entry[j];
        if (!used) continue;
        struct partition *p = &result.parts[result.count];
        p->first = read_le64(entry + 32); p->last = read_le64(entry + 40); p->slot = i + 1;
        if (p->first < first || p->last > last || p->first > p->last) return false;
        for (unsigned j = 0; j < result.count; j++) {
            const struct partition *other = &result.parts[j];
            if (p->first <= other->last && other->first <= p->last) return false;
            if (!memcmp(entry + 16, table + (other->slot - 1) * stride + 16, 16)) return false;
        }
        for (unsigned j = 0; j < 36; j++) {
            unsigned c = entry[56 + 2 * j] | (unsigned)entry[57 + 2 * j] << 8;
            if (!c) break;
            p->name[j] = c >= 32 && c < 127 ? (char)c : '?';
        }
        result.count++;
    }
    *out = result;
    return true;
}
