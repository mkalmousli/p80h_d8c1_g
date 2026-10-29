/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/rom/rom.h"
#include <errno.h>
#include <stdio.h>
#include <time.h>

int rom_info_cli(int wait, int reset)
{
    struct timespec start, now, pause = {0, 100000000};
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (wait) fputs("Waiting up to 60 seconds for MT8163 ROM/preloader...\n", stderr);
    do {
        struct rom_usb usb = {0};
        usb.fd = rom_usb_open(&usb.endpoints);
        if (usb.fd >= 0) {
            struct rom_transport transport = {&usb, rom_usb_transfer};
            struct rom_information info;
            int failed = rom_probe(&transport, &info);
            if (!failed) {
                printf("MT8163 ROM/preloader: hardware=%04x subcode=%04x software=%04x security=%08x\n",
                       info.hardware, info.subcode, info.software, info.config);
                printf("SBC=%u SLA=%u DAA=%u; identity queries and flash writes are absent.\n",
                       !!(info.config & 1), !!(info.config & 2), !!(info.config & 4));
                int result = reset ? rom_wdt_reset(&transport, &info) : 0;
                rom_usb_close(usb.fd, &usb.endpoints);
                if (reset) fputs(result ? "WDT reset request failed.\n" : "WDT reset sent; verify normal boot.\n", stderr);
                return result;
            }
            rom_usb_close(usb.fd, &usb.endpoints);
            fprintf(stderr, "ROM query incomplete: stage=%d chip=%04x\n", failed, info.chip);
            if (info.chip && info.chip != 0x8163) return 1;
        } else if (errno == EACCES || errno == EPERM || errno == EEXIST) return 1;
        if (!wait) break;
        nanosleep(&pause, NULL); clock_gettime(CLOCK_MONOTONIC, &now);
    } while (now.tv_sec - start.tv_sec < 60);
    fputs("No successful MT8163 ROM/preloader probe.\n", stderr); return 1;
}
