/* SPDX-License-Identifier: MIT */
#ifndef P80H_ROM_H
#define P80H_ROM_H
#include <stddef.h>
#include <stdint.h>
struct rom_endpoint { unsigned interface, input, output; };
struct rom_transport { void *context; int (*transfer)(void *, int writing, unsigned char *, size_t); };
struct rom_information { uint16_t chip, hardware, subcode, software; uint32_t config; };
/* Probe reads only MTK handshake/chip/security facts; it never reads serial identities. */
int rom_descriptors(const unsigned char *, size_t, struct rom_endpoint *);
int rom_probe(struct rom_transport *, struct rom_information *);
int rom_wdt_reset(struct rom_transport *, const struct rom_information *);
int rom_info_cli(int wait, int reset);
int rom_usb_open(struct rom_endpoint *);
int rom_usb_transfer(void *, int, unsigned char *, size_t);
void rom_usb_close(int, const struct rom_endpoint *);
struct rom_usb {
    int fd; struct rom_endpoint endpoints;
    unsigned char receive[1024]; size_t position, available;
};
#endif
