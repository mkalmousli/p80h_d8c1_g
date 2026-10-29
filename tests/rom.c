/* SPDX-License-Identifier: MIT */
#include "src/rom/rom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct step { int writing; size_t size; unsigned char bytes[8]; };
struct scenario { struct step *steps; unsigned at, count; };

static int transfer(void *context, int writing, unsigned char *bytes, size_t size)
{
    struct scenario *s = context;
    if (s->at == s->count) return 1;
    struct step *step = &s->steps[s->at++];
    if (step->writing != writing || step->size != size) return 1;
    if (writing) return memcmp(step->bytes, bytes, size) != 0;
    memcpy(bytes, step->bytes, size); return 0;
}

int main(void)
{
    unsigned char config[] = {9,2,32,0,1,1,0,0x80,50,9,4,0,0,2,0xff,0,0,0,
        7,5,1,2,64,0,0,7,5,0x81,2,64,0,0};
    struct rom_endpoint endpoints;
    assert(!rom_descriptors(config, sizeof(config), &endpoints));
    assert(endpoints.interface == 0 && endpoints.input == 0x81 && endpoints.output == 1);
    assert(rom_descriptors(config, sizeof(config) - 1, &endpoints));
    config[0] = 0; assert(rom_descriptors(config, sizeof(config), &endpoints)); config[0] = 9;
    config[12] = 1; assert(rom_descriptors(config, sizeof(config), &endpoints));
    struct step steps[] = {
        {1,1,{0xa0}}, {0,1,{0x5f}}, {1,1,{0x0a}}, {0,1,{0xf5}},
        {1,1,{0x50}}, {0,1,{0xaf}}, {1,1,{0x05}}, {0,1,{0xfa}},
        {1,1,{0xfd}}, {0,1,{0xfd}}, {0,4,{0x81,0x63,0,1}},
        {1,1,{0xfc}}, {0,1,{0xfc}}, {0,8,{0,2,0,1,0,3,0,0}},
        {1,1,{0xd8}}, {0,1,{0xd8}}, {0,6,{0,0,0,5,0,0}}};
    struct scenario s = {steps, 0, sizeof(steps) / sizeof(steps[0])};
    struct rom_transport t = {&s, transfer};
    struct rom_information info;
    assert(!rom_probe(&t, &info) && s.at == s.count);
    assert(info.chip == 0x8163 && info.hardware == 1 && info.software == 3 && info.config == 5);
    s.at = 0; steps[1].bytes[0] = 0;
    assert(rom_probe(&t, &info) && s.at == 2); steps[1].bytes[0] = 0x5f;
    s.at = 0; steps[10].bytes[1] = 0x65;
    assert(rom_probe(&t, &info) && s.at == 11);
    puts("PASS: ROM endpoint discovery, exact query sequence, bad echo and wrong-chip refusal");
    return 0;
}
