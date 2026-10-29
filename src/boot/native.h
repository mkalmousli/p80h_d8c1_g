/* SPDX-License-Identifier: MIT */
#ifndef P80H_NATIVE_H
#define P80H_NATIVE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct native_image { uint64_t offset, memory_size, flags; };
struct memory_span { uint64_t start, size; };
/* Modern little-endian ARM64 Image bounds only; no decompression, execution or kexec. */
bool native_parse(const unsigned char *header, size_t length, uint64_t file_size,
                  struct native_image *);
/* Validate RAM/reserved overlap before placement; this is not a board loader or handoff. */
bool native_layout(struct memory_span ram, struct memory_span kernel,
                   struct memory_span dtb, struct memory_span initrd,
                   const struct memory_span *reserved, size_t reserved_count);
#endif
