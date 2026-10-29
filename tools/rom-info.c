/* SPDX-License-Identifier: MIT */
#include "src/rom/rom.h"
#include <stdio.h>

int rom_info_cli(void)
{
    struct rom_usb usb;
    usb.fd = rom_usb_open(&usb.endpoints);
    if (usb.fd < 0) return 1;
    struct rom_transport transport = {&usb, rom_usb_transfer};
    struct rom_information info;
    int failed = rom_probe(&transport, &info);
    rom_usb_close(usb.fd, &usb.endpoints);
    if (failed) { fputs("ROM handshake/query failed or target is not MT8163.\n", stderr); return 1; }
    printf("MT8163 ROM/preloader: hardware=%04x subcode=%04x software=%04x security=%08x\n",
           info.hardware, info.subcode, info.software, info.config);
    printf("SBC=%u SLA=%u DAA=%u; no identities read and no flash/memory writes performed.\n",
           !!(info.config & 1), !!(info.config & 2), !!(info.config & 4));
    return 0;
}
