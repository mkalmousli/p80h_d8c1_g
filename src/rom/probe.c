/* SPDX-License-Identifier: MIT */
#include "rom.h"
#include <string.h>

static uint16_t u16(const unsigned char *p) { return (uint16_t)((unsigned)p[0] << 8 | p[1]); }
static uint32_t u32(const unsigned char *p) { return (uint32_t)u16(p) << 16 | u16(p + 2); }

static int query(struct rom_transport *t, unsigned char command, unsigned char *reply, size_t size)
{
    unsigned char echo;
    return t->transfer(t->context, 1, &command, 1) || t->transfer(t->context, 0, &echo, 1) ||
        echo != command || t->transfer(t->context, 0, reply, size);
}

/* Only synchronization and non-identifying hardware/security queries are allowed. */
int rom_probe(struct rom_transport *t, struct rom_information *info)
{
    const unsigned char sync[] = {0xa0, 0x0a, 0x50, 0x05};
    unsigned char reply[8], byte;
    *info = (struct rom_information){0};
    for (unsigned i = 0; i < sizeof(sync); i++) {
        byte = sync[i];
        if (t->transfer(t->context, 1, &byte, 1) || t->transfer(t->context, 0, reply, 1) ||
            reply[0] != (unsigned char)(sync[i] ^ 0xffU)) return 1;
    }
    if (query(t, 0xfd, reply, 4)) return 2;
    info->chip = u16(reply); info->hardware = u16(reply + 2);
    if (info->chip != 0x8163) return 3;
    if (query(t, 0xfc, reply, 8) || u16(reply + 6)) return 4;
    info->subcode = u16(reply); info->software = u16(reply + 4);
    if (query(t, 0xd8, reply, 6) || u16(reply + 4)) return 5;
    info->config = u32(reply);
    return 0;
}
