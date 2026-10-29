/* SPDX-License-Identifier: MIT */
#include "native.h"

static bool valid(struct memory_span span)
{
    return span.size && span.start <= UINT64_MAX - span.size;
}

static bool overlaps(struct memory_span a, struct memory_span b)
{
    return a.start < b.start + b.size && b.start < a.start + a.size;
}

/* DTB placement follows ARM64 alignment/size limits. initrd is optional. */
bool native_layout(struct memory_span ram, struct memory_span kernel,
                   struct memory_span dtb, struct memory_span initrd,
                   const struct memory_span *reserved, size_t count)
{
    if (!valid(ram) || !valid(kernel) || !valid(dtb) ||
        (dtb.start & 7) || dtb.size > 2 * 1024 * 1024 || count > 64 ||
        (count && !reserved)) return false;
    struct memory_span regions[] = {kernel, dtb, initrd};
    size_t region_count = initrd.size ? 3 : 2;
    for (size_t i = 0; i < region_count; i++) {
        struct memory_span region = regions[i];
        if (!valid(region) || region.start < ram.start ||
            region.start + region.size > ram.start + ram.size) return false;
        for (size_t j = 0; j < i; j++) if (overlaps(region, regions[j])) return false;
        for (size_t j = 0; j < count; j++) {
            if (!valid(reserved[j]) || overlaps(region, reserved[j])) return false;
        }
    }
    return true;
}
