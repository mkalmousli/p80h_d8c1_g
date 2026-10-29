/* SPDX-License-Identifier: MIT */
#include "menu.h"

bool menu_init(struct boot_menu *m, const struct boot_entry *entries,
               size_t count, size_t def, uint32_t timeout)
{
    if (!m) return false;
    *m = (struct boot_menu){0};
    if (!entries || !count || count > 8 || def >= count ||
        !entries[def].available) return false;
    for (size_t i = 0; i < count; i++)
        if (!entries[i].label || entries[i].kind > BOOT_RECOVERY ||
            entries[i].kind < BOOT_LINUX) return false;
    *m = (struct boot_menu){entries, count, def, timeout, timeout != 0};
    return true;
}

int menu_step(struct boot_menu *m, enum menu_event event, uint32_t elapsed)
{
    if (!m || !m->entries || !m->count || m->selected >= m->count) return -1;
    if (event == MENU_TICK) {
        if (!m->countdown) return -1;
        if (elapsed < m->remaining_ms) {
            m->remaining_ms -= elapsed;
            return -1;
        }
        m->remaining_ms = 0;
        m->countdown = false;
        return m->entries[m->selected].available ? (int)m->selected : -1;
    }
    if (event < MENU_UP || event > MENU_CANCEL) return -1;
    m->countdown = false;
    if (event == MENU_SELECT)
        return m->entries[m->selected].available ? (int)m->selected : -1;
    if (event != MENU_UP && event != MENU_DOWN) return -1;
    for (size_t i = 0; i < m->count; i++) {
        m->selected = event == MENU_DOWN ? (m->selected + 1) % m->count :
            (m->selected + m->count - 1) % m->count;
        if (m->entries[m->selected].available) break;
    }
    return -1;
}
