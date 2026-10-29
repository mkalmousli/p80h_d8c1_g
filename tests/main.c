/* SPDX-License-Identifier: MIT */
#include "src/boot/menu.h"
#include "tools/image.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void menu_tests(void)
{
    struct boot_menu m;
    struct boot_entry entries[] = {{"Alpine", BOOT_LINUX, true},
        {"Android", BOOT_ANDROID, false}, {"Recovery", BOOT_RECOVERY, true}};
    assert(!menu_init(NULL, entries, 3, 0, 1000));
    assert(!menu_init(&m, NULL, 3, 0, 1000));
    assert(!menu_init(&m, entries, 0, 0, 1000));
    assert(!menu_init(&m, entries, 3, 1, 1000));
    assert(menu_step(&m, MENU_SELECT, 0) == -1);
    assert(menu_init(&m, entries, 3, 0, 1000));
    assert(menu_step(&m, MENU_TICK, 999) == -1);
    assert(menu_step(&m, MENU_TICK, 1) == 0);
    assert(menu_step(&m, MENU_TICK, 1) == -1);
    assert(menu_init(&m, entries, 3, 0, 1000));
    assert(menu_step(&m, MENU_DOWN, 0) == -1 && m.selected == 2);
    assert(menu_step(&m, MENU_TICK, UINT32_MAX) == -1);
    assert(menu_step(&m, MENU_DOWN, 0) == -1 && m.selected == 0);
    assert(menu_step(&m, MENU_UP, 0) == -1 && m.selected == 2);
    assert(menu_step(&m, MENU_SELECT, 0) == 2);
    assert(menu_init(&m, entries, 3, 0, 0));
    assert(menu_step(&m, MENU_TICK, UINT32_MAX) == -1);
    assert(menu_init(&m, entries, 3, 0, 1000));
    assert(menu_step(&m, MENU_CANCEL, 0) == -1 && !m.countdown);
}

static void put32(unsigned char *p, uint32_t v)
{
    for (unsigned i = 0; i < 4; i++) p[i] = (unsigned char)(v >> (i * 8));
}

static void image_tests(void)
{
    unsigned char h[1632] = {0};
    struct boot_image image;
    memcpy(h, "ANDROID!", 8);
    put32(h + 8, 4097); put32(h + 16, 100); put32(h + 36, 2048);
    assert(image_parse(h, sizeof(h), 10240, &image));
    assert(image.end == 10240 && image.kernel == 4097);
    assert(!image_parse(h, sizeof(h), 10239, &image));
    assert(!image_parse(h, 48, 10240, &image));
    assert(!image_parse(NULL, sizeof(h), 10240, &image));
    assert(!image_parse(h, sizeof(h), 10240, NULL));
    put32(h + 40, 1);
    assert(!image_parse(h, sizeof(h), 10240, &image));
    put32(h + 40, 0); put32(h + 36, 3000);
    assert(!image_parse(h, sizeof(h), UINT64_MAX, &image));
    put32(h + 36, 2048); put32(h + 8, UINT32_MAX);
    assert(!image_parse(h, sizeof(h), UINT32_MAX, &image));
    assert(image_parse(h, sizeof(h), UINT64_MAX, &image));
    assert(image.end > UINT32_MAX);
    for (size_t n = 0; n < sizeof(h); n++) assert(!image_parse(h, n, UINT64_MAX, &image));
    h[0] = 0;
    assert(!image_parse(h, sizeof(h), UINT64_MAX, &image));
}

int main(void)
{
    menu_tests(); image_tests();
    puts("PASS: boot policy and malformed/truncated/overflow image cases");
    return 0;
}
