/* SPDX-License-Identifier: MIT */
#include "src/host/gpt.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put32(unsigned char *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; i++) p[i] = (unsigned char)(value >> (8 * i));
}

static void checksums(unsigned char *disk, unsigned entries)
{
    put32(disk + 512 + 80, entries);
    put32(disk + 512 + 88, gpt_crc32(disk + 1024, entries * 128));
    put32(disk + 512 + 16, 0);
    put32(disk + 512 + 16, gpt_crc32(disk + 512, 92));
}

int main(void)
{
    unsigned char disk[17408] = {0};
    unsigned char *h = disk + 512, *entry = disk + 1024;
    struct partition_table table;
    assert(gpt_crc32((const unsigned char *)"123456789", 9) == 0xcbf43926);
    memcpy(h, "EFI PART", 8);
    put32(h + 8, 0x10000); put32(h + 12, 92); put32(h + 24, 1);
    put32(h + 32, 9999); put32(h + 40, 34); put32(h + 48, 9900);
    put32(h + 72, 2); put32(h + 84, 128);
    entry[0] = 1; entry[16] = 1;
    put32(entry + 32, 34); put32(entry + 40, 100); entry[56] = 'x';
    checksums(disk, 1);
    assert(gpt_parse(disk, sizeof(disk), &table));
    assert(table.count == 1 && !strcmp(table.parts[0].name, "x"));
    assert(!gpt_parse(disk, 1024, &table));
    h[16] ^= 1; assert(!gpt_parse(disk, sizeof(disk), &table)); h[16] ^= 1;
    entry[56] ^= 1; assert(!gpt_parse(disk, sizeof(disk), &table)); entry[56] ^= 1;
    memcpy(entry + 128, entry, 128); entry[128 + 16] = 2;
    checksums(disk, 2);
    assert(!gpt_parse(disk, sizeof(disk), &table));
    put32(entry + 128 + 32, 101); put32(entry + 128 + 40, 200);
    checksums(disk, 2);
    assert(gpt_parse(disk, sizeof(disk), &table) && table.count == 2);
    puts("PASS: GPT checksums, bounds, duplicate identities and overlap protection");
    return 0;
}
