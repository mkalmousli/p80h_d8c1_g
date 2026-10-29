/* SPDX-License-Identifier: MIT */
#ifndef P80H_IMAGE_H
#define P80H_IMAGE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
struct boot_image { uint32_t kernel, ramdisk, second, page; uint64_t end; };
/* Android legacy v0 only. Validates extents, never exposes board/cmdline/IDs. */
bool image_parse(const unsigned char *, size_t header_size, uint64_t file_size,
                 struct boot_image *);
#endif
