/* SPDX-License-Identifier: MIT */
#include "rom.h"

/* Pick a zero-altsetting interface with both bulk endpoints, never an interrupt EP. */
int rom_descriptors(const unsigned char *data, size_t size, struct rom_endpoint *out)
{
    *out = (struct rom_endpoint){.interface = 256};
    unsigned interface = 256, input = 0, output = 0;
    for (size_t at = 0; at < size;) {
        if (size - at < 2 || data[at] < 2 || data[at] > size - at) return 1;
        unsigned length = data[at], type = data[at + 1];
        if (type == 4 && length >= 9) {
            interface = data[at + 3] == 0 ? data[at + 2] : 256;
            input = output = 0;
        }
        if (type == 5 && length >= 7 && interface < 256 && (data[at + 3] & 3) == 2) {
            unsigned address = data[at + 2];
            if (!(address & 15) || !(data[at + 4] | data[at + 5])) return 1;
            if (address & 0x80) input = address; else output = address;
            if (input && output) { *out = (struct rom_endpoint){interface, input, output}; return 0; }
        }
        at += length;
    }
    return 1;
}
