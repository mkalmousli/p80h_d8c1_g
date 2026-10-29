/* SPDX-License-Identifier: MIT */
#include "rom.h"
#include <string.h>

static void put32(unsigned char *p, uint32_t n)
{
    for (unsigned i = 0; i < 4; i++) p[i] = (unsigned char)(n >> (24 - 8 * i));
}

static int echo(struct rom_transport *t, unsigned char *bytes, size_t n)
{
    unsigned char reply[4];
    return t->transfer(t->context, 1, bytes, n) || t->transfer(t->context, 0, reply, n) ||
        memcmp(bytes, reply, n);
}

static int begin(struct rom_transport *t, unsigned char command, uint32_t address)
{
    unsigned char data[4], status[2];
    if (echo(t, &command, 1)) return 1;
    put32(data, address);
    if (echo(t, data, 4)) return 1;
    put32(data, 1);
    return echo(t, data, 4) || t->transfer(t->context, 0, status, 2) || status[0] || status[1];
}

/* Restricted reset only: never accept arbitrary memory addresses from the CLI. */
int rom_wdt_reset(struct rom_transport *t, const struct rom_information *info)
{
    if (info->chip != 0x8163 || (info->config & 0x60)) return 1;
    unsigned char data[4], status[2];
    if (begin(t, 0xd1, 0x10007000) || t->transfer(t->context, 0, data, 4) ||
        t->transfer(t->context, 0, status, 2) || status[0] || status[1]) return 1;
    uint32_t mode = (uint32_t)data[0] << 24 | (uint32_t)data[1] << 16 |
        (uint32_t)data[2] << 8 | data[3];
    /* Linux mtk_wdt's reset contract: select reset instead of IRQ, then SWRST. */
    put32(data, (mode & ~8U) | 0x22000000U);
    if (begin(t, 0xd4, 0x10007000) || echo(t, data, 4) ||
        t->transfer(t->context, 0, status, 2) || status[0] || status[1]) return 1;
    if (begin(t, 0xd4, 0x10007014)) return 1;
    put32(data, 0x1209);
    /* Reset may disconnect USB before the final echo. Boot return proves success. */
    return t->transfer(t->context, 1, data, 4);
}
