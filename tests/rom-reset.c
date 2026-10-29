/* SPDX-License-Identifier: MIT */
#include "src/rom/rom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct step { int writing; size_t size; unsigned char bytes[4]; };
struct scenario { unsigned at; int corrupt; };
static const struct step steps[] = {
    {1,1,{0xd1}}, {0,1,{0xd1}}, {1,4,{0x10,0,0x70,0}}, {0,4,{0x10,0,0x70,0}},
    {1,4,{0,0,0,1}}, {0,4,{0,0,0,1}}, {0,2,{0}}, {0,4,{0,0,0,8}}, {0,2,{0}},
    {1,1,{0xd4}}, {0,1,{0xd4}}, {1,4,{0x10,0,0x70,0}}, {0,4,{0x10,0,0x70,0}},
    {1,4,{0,0,0,1}}, {0,4,{0,0,0,1}}, {0,2,{0}}, {1,4,{0x22,0,0,0}}, {0,4,{0x22,0,0,0}}, {0,2,{0}},
    {1,1,{0xd4}}, {0,1,{0xd4}}, {1,4,{0x10,0,0x70,0x14}}, {0,4,{0x10,0,0x70,0x14}},
    {1,4,{0,0,0,1}}, {0,4,{0,0,0,1}}, {0,2,{0}}, {1,4,{0,0,0x12,9}}};

static int transfer(void *context, int writing, unsigned char *data, size_t size)
{
    struct scenario *s = context;
    assert(s->at < sizeof(steps) / sizeof(steps[0]));
    const struct step *step = &steps[s->at++];
    assert(writing == step->writing && size == step->size);
    if (writing) return memcmp(data, step->bytes, size) != 0;
    memcpy(data, step->bytes, size);
    if (s->corrupt && s->at == 2) data[0] ^= 1;
    return 0;
}

int main(void)
{
    struct scenario s = {0};
    struct rom_transport t = {&s, transfer};
    struct rom_information info = {.chip = 0x8163};
    assert(!rom_wdt_reset(&t, &info) && s.at == sizeof(steps) / sizeof(steps[0]));
    s.at = 0; info.chip = 0x8165; assert(rom_wdt_reset(&t, &info) && !s.at);
    info.chip = 0x8163; info.config = 0x40; assert(rom_wdt_reset(&t, &info) && !s.at);
    info.config = 0; s.corrupt = 1;
    assert(rom_wdt_reset(&t, &info) && s.at == 2);
    puts("PASS: restricted watchdog reset sequence, security gates and bad-echo refusal");
    return 0;
}
