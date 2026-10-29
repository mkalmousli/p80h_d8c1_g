/* SPDX-License-Identifier: MIT */
#include <dirent.h>
#include <stdio.h>
#include <string.h>

static unsigned attribute(const char *name, const char *key)
{
    char path[512];
    unsigned value = 0;
    int n = snprintf(path, sizeof(path), "/sys/bus/usb/devices/%s/%s", name, key);
    if (n < 0 || (size_t)n >= sizeof(path)) return 0;
    FILE *f = fopen(path, "r");
    if (f) { if (fscanf(f, "%x", &value) != 1) value = 0; fclose(f); }
    return value;
}

int usb_list(void)
{
    DIR *dir = opendir("/sys/bus/usb/devices");
    struct dirent *entry;
    unsigned count = 0;
    if (!dir) { perror("USB sysfs"); return 1; }
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.' || strchr(entry->d_name, ':')) continue;
        unsigned vendor = attribute(entry->d_name, "idVendor");
        unsigned product = attribute(entry->d_name, "idProduct");
        if (vendor != 0x0e8d && vendor != 0x18d1) continue;
        printf("USB candidate %04x:%04x — identity not established\n", vendor, product);
        count++;
    }
    closedir(dir);
    if (!count) puts("No MediaTek/Google USB candidates found.");
    return 0;
}
