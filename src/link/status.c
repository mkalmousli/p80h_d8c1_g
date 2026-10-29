/* SPDX-License-Identifier: MIT */
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t number(const char *path, uint32_t fallback)
{
    FILE *f = fopen(path, "r");
    unsigned value;
    if (!f) return fallback;
    int ok = fscanf(f, "%u", &value) == 1;
    fclose(f); return ok ? value : fallback;
}

/* Fixed 32-byte status record. Availability flags distinguish missing readings. */
int serve_status(struct wire_message *reply)
{
    reply->body = calloc(1, 32);
    if (!reply->body) return WIRE_IO;
    reply->size = 32;
    FILE *f = fopen("/proc/uptime", "r");
    double uptime = 0;
    if (f) { if (fscanf(f, "%lf", &uptime) != 1) uptime = 0; fclose(f); }
    wire_put64(reply->body, (uint64_t)(uptime * 1000));
    f = fopen("/proc/meminfo", "r");
    char line[256];
    unsigned long long memory = 0;
    while (f && fgets(line, sizeof(line), f))
        if (sscanf(line, "MemAvailable: %llu", &memory) == 1) break;
    if (f) fclose(f);
    wire_put64(reply->body + 8, (uint64_t)memory * 1024);
    uint32_t voltage = number("/sys/class/power_supply/battery/voltage_now", UINT32_MAX);
    uint32_t capacity = number("/sys/class/power_supply/battery/capacity", UINT32_MAX);
    wire_put32(reply->body + 16, voltage); wire_put32(reply->body + 20, capacity);
    wire_put32(reply->body + 24, (voltage != UINT32_MAX ? 1U : 0U) | (capacity <= 100 ? 2U : 0U));
    return WIRE_OK;
}
