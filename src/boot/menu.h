/* SPDX-License-Identifier: MIT */
#ifndef P80H_MENU_H
#define P80H_MENU_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum boot_kind { BOOT_LINUX, BOOT_ANDROID, BOOT_RECOVERY };
struct boot_entry { const char *label; enum boot_kind kind; bool available; };
enum menu_event { MENU_UP, MENU_DOWN, MENU_SELECT, MENU_TICK, MENU_CANCEL };
struct boot_menu {
    const struct boot_entry *entries;
    size_t count, selected;
    uint32_t remaining_ms;
    bool countdown;
};

/* Returns false for an invalid default; never silently boots a different OS. */
bool menu_init(struct boot_menu *, const struct boot_entry *, size_t,
               size_t default_entry, uint32_t timeout_ms);
/* Returns an entry index on selection, -1 for no action. No hardware I/O. */
int menu_step(struct boot_menu *, enum menu_event, uint32_t elapsed_ms);
#endif
