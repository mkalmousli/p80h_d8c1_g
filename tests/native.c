/* SPDX-License-Identifier: MIT */
#include "src/boot/native.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put64(unsigned char *p, uint64_t n)
{
    for (unsigned i = 0; i < 8; i++) p[i] = (unsigned char)(n >> (8 * i));
}

int main(void)
{
    unsigned char header[64] = {0};
    memcpy(header + 56, "ARM\x64", 4);
    put64(header + 8, 0x80000); put64(header + 16, 0x400000);
    struct native_image image;
    assert(native_parse(header, 64, 0x300000, &image));
    assert(image.offset == 0x80000 && image.memory_size == 0x400000);
    for (size_t i = 0; i < 64; i++) assert(!native_parse(header, i, 0x300000, &image));
    assert(!native_parse(header, 64, 0x500000, &image));
    put64(header + 24, 1);
    assert(!native_parse(header, 64, 0x300000, &image));
    put64(header + 24, 0); put64(header + 8, UINT64_MAX);
    assert(!native_parse(header, 64, 0x300000, &image));
    struct memory_span ram = {0x40000000, 0x40000000};
    struct memory_span kernel = {0x40080000, 0x400000};
    struct memory_span dtb = {0x41000000, 0x10000};
    struct memory_span initrd = {0x42000000, 0x100000};
    struct memory_span reserved = {0x43000000, 0x1000};
    assert(native_layout(ram, kernel, dtb, initrd, &reserved, 1));
    assert(native_layout(ram, kernel, dtb, (struct memory_span){0}, NULL, 0));
    assert(!native_layout(ram, kernel, kernel, initrd, NULL, 0));
    assert(!native_layout(ram, kernel, dtb, initrd, &kernel, 1));
    assert(!native_layout(ram, kernel, (struct memory_span){0x41000001, 0x100}, initrd, NULL, 0));
    assert(!native_layout(ram, kernel, (struct memory_span){0x41000000, 0x300000}, initrd, NULL, 0));
    assert(!native_layout(ram, kernel, dtb, (struct memory_span){UINT64_MAX - 8, 32}, NULL, 0));
    puts("PASS: native ARM64 Image bounds and reserved-memory placement checks");
    return 0;
}
