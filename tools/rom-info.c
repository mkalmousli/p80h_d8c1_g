/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/rom/rom.h"
#include <errno.h>
#include <stdio.h>
#include <time.h>

int rom_info_cli(int wait)
{
    struct timespec start, now, pause = {0, 100000000};
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (wait) fputs("Waiting up to 60 seconds for MT8163 ROM/preloader...\n", stderr);
    do {
        struct rom_usb usb;
        usb.fd = rom_usb_open(&usb.endpoints);
        if (usb.fd >= 0) {
            struct rom_transport transport = {&usb, rom_usb_transfer};
            struct rom_information info;
            int failed = rom_probe(&transport, &info);
            rom_usb_close(usb.fd, &usb.endpoints);
            if (!failed) {
                printf("MT8163 ROM/preloader: hardware=%04x subcode=%04x software=%04x security=%08x\n",
                       info.hardware, info.subcode, info.software, info.config);
                printf("SBC=%u SLA=%u DAA=%u; no identities read or flash/memory writes.\n",
                       !!(info.config & 1), !!(info.config & 2), !!(info.config & 4));
                return 0;
            }
            if (info.chip && info.chip != 0x8163) return 1;
        } else if (errno == EACCES || errno == EPERM || errno == EEXIST) return 1;
        if (!wait) break;
        nanosleep(&pause, NULL); clock_gettime(CLOCK_MONOTONIC, &now);
    } while (now.tv_sec - start.tv_sec < 60);
    fputs("No successful MT8163 ROM/preloader probe.\n", stderr); return 1;
}
