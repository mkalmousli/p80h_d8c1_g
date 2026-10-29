/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "src/rom/rom.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
int rom_usb_claim(int fd, struct rom_endpoint *);

static unsigned number(const char *device, const char *key, int base)
{
    char path[512];
    int n = snprintf(path, sizeof(path), "/sys/bus/usb/devices/%s/%s", device, key);
    if (n < 0 || (size_t)n >= sizeof(path)) return 0;
    FILE *f = fopen(path, "r");
    unsigned value = 0;
    if (f) { if (fscanf(f, base == 16 ? "%x" : "%u", &value) != 1) value = 0; fclose(f); }
    return value;
}

/* Refuse ambiguous candidates and never touch the running Linux gadget PID. */
int rom_usb_open(struct rom_endpoint *endpoints)
{
    DIR *d = opendir("/sys/bus/usb/devices");
    struct dirent *e;
    char path[128] = {0};
    unsigned count = 0, selected_pid = 0;
    if (!d) return -1;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.' || strchr(e->d_name, ':')) continue;
        unsigned vendor = number(e->d_name, "idVendor", 16), pid = number(e->d_name, "idProduct", 16);
        if (vendor != 0x0e8d || (pid != 0x0003 && pid != 0x2000)) continue;
        unsigned bus = number(e->d_name, "busnum", 10), device = number(e->d_name, "devnum", 10);
        if (!bus || !device || bus > 999 || device > 999) continue;
        count++; selected_pid = pid; snprintf(path, sizeof(path), "/dev/bus/usb/%03u/%03u", bus, device);
    }
    closedir(d);
    if (count != 1) {
        errno = count ? EEXIST : ENODEV;
        if (count) fputs("Ambiguous MTK ROM/preloader devices; refusing to select one.\n", stderr);
        return -1;
    }
    fprintf(stderr, "MTK USB candidate 0e8d:%04x; initializing bulk transport.\n", selected_pid);
    int fd = open(path, O_RDWR | O_CLOEXEC);
    if (fd < 0) { perror("ROM USB access"); return -1; }
    if (rom_usb_claim(fd, endpoints)) { rom_usb_close(fd, endpoints); return -1; }
    return fd;
}
