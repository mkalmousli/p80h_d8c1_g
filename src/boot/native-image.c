/* SPDX-License-Identifier: MIT */
#include "native.h"
#include <string.h>

static uint64_t little64(const unsigned char *p)
{
    uint64_t result = 0;
    for (unsigned i = 0; i < 8; i++) result |= (uint64_t)p[i] << (8 * i);
    return result;
}

/* image_size reserves memory including BSS; it can exceed the bytes on disk. */
bool native_parse(const unsigned char *h, size_t size, uint64_t file_size,
                  struct native_image *result)
{
    if (!result) return false;
    *result = (struct native_image){0};
    if (!h || size < 64 || file_size < 64 || memcmp(h + 56, "ARM\x64", 4)) return false;
    struct native_image image = {little64(h + 8), little64(h + 16), little64(h + 24)};
    /* Reject legacy unbounded size and big-endian/reserved flags for this port. */
    if (image.memory_size < file_size || image.memory_size < 64 || (image.flags & ~14ULL))
        return false;
    if (image.offset > UINT64_MAX - image.memory_size) return false;
    for (unsigned i = 32; i < 56; i++) if (h[i]) return false;
    *result = image;
    return true;
}
