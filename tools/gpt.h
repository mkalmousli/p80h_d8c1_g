/* SPDX-License-Identifier: MIT */
#ifndef P80H_GPT_H
#define P80H_GPT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
struct partition { uint64_t first, last; unsigned slot; char name[37]; };
struct partition_table { struct partition parts[128]; unsigned count; };
uint32_t gpt_crc32(const unsigned char *, size_t);
/* Primary GPT snapshot, 512-byte sectors, <=128 entries. Never exports GUIDs. */
bool gpt_parse(const unsigned char *, size_t, struct partition_table *);
int gpt_inspect(const char *path);
#endif
