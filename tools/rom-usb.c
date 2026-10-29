/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/rom/rom.h"
#include <errno.h>
#include <linux/usbdevice_fs.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int rom_usb_claim(int fd, struct rom_endpoint *out)
{
    unsigned char config[4096];
    *out = (struct rom_endpoint){.interface = 256};
    struct usbdevfs_ctrltransfer get = {.bRequestType = 0x80, .bRequest = 6,
        .wValue = 0x200, .wLength = 9, .timeout = 1000, .data = config};
    int size = ioctl(fd, USBDEVFS_CONTROL, &get);
    if (size != 9) { fprintf(stderr, "ROM config header read failed: bytes=%d errno=%d\n", size, errno); return 1; }
    unsigned total = config[2] | (unsigned)config[3] << 8;
    if (total < 9 || total > sizeof(config)) { fprintf(stderr, "ROM config length invalid: %u\n", total); return 1; }
    get.wLength = (unsigned short)total;
    size = ioctl(fd, USBDEVFS_CONTROL, &get);
    if (size != (int)total || rom_descriptors(config, (size_t)size, out)) {
        fprintf(stderr, "ROM bulk descriptor discovery failed: bytes=%d expected=%u errno=%d\n", size, total, errno);
        return 1;
    }
    unsigned interface = out->interface;
    if (ioctl(fd, USBDEVFS_CLAIMINTERFACE, &interface)) {
        struct usbdevfs_disconnect_claim claim = {.interface = interface};
        if (ioctl(fd, USBDEVFS_DISCONNECT_CLAIM, &claim)) { perror("ROM interface claim"); return 1; }
    }
    /* CDC line setup is volatile; vendor-class ROMs may legitimately stall it. */
    unsigned char coding[] = {0, 0x10, 0x0e, 0, 0, 0, 8};
    struct usbdevfs_ctrltransfer line = {.bRequestType = 0x21, .bRequest = 0x20,
        .wIndex = interface ? interface - 1 : 0, .wLength = 7, .timeout = 250, .data = coding};
    (void)ioctl(fd, USBDEVFS_CONTROL, &line);
    line.bRequest = 0x22; line.wValue = 3; line.wLength = 0; line.data = NULL;
    (void)ioctl(fd, USBDEVFS_CONTROL, &line);
    return 0;
}

int rom_usb_transfer(void *context, int writing, unsigned char *data, size_t size)
{
    struct rom_usb *usb = context;
    while (size) {
        struct usbdevfs_bulktransfer bulk = {.ep = writing ? usb->endpoints.output : usb->endpoints.input,
            .len = (unsigned)size, .timeout = 1000, .data = data};
        int n = ioctl(usb->fd, USBDEVFS_BULK, &bulk);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0 || (size_t)n > size) {
            fprintf(stderr, "ROM bulk %s failed: endpoint=%02x bytes=%d errno=%d\n",
                    writing ? "write" : "read", bulk.ep, n, errno); return 1;
        }
        size -= (size_t)n; data += n;
    }
    return 0;
}

void rom_usb_close(int fd, const struct rom_endpoint *endpoints)
{
    if (endpoints->interface < 256) {
        unsigned interface = endpoints->interface;
        (void)ioctl(fd, USBDEVFS_RELEASEINTERFACE, &interface);
    }
    close(fd);
}
