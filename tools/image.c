/* SPDX-License-Identifier: MIT */
#include "image.h"
#include <string.h>

static uint32_t le32(const unsigned char *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
        (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static uint64_t aligned(uint32_t size, uint32_t page)
{
    return ((uint64_t)size + page - 1) / page * page;
}

bool image_parse(const unsigned char *h, size_t n, uint64_t size,
                 struct boot_image *out)
{
    if (!out) return false;
    *out = (struct boot_image){0};
    if (!h || n < 1632 || memcmp(h, "ANDROID!", 8)) return false;
    uint32_t page = le32(h + 36);
    if (page < 2048 || page > 65536 || (page & (page - 1))) return false;
    /* Offset 40 was dt_size in older MediaTek images. Reject either extension. */
    if (le32(h + 40) || le32(h + 44)) return false;
    struct boot_image image = {le32(h + 8), le32(h + 16), le32(h + 24), page, 0};
    if (!image.kernel) return false;
    image.end = page + aligned(image.kernel, page) +
        aligned(image.ramdisk, page) + aligned(image.second, page);
    if (image.end > size) return false;
    *out = image;
    return true;
}
