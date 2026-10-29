/* SPDX-License-Identifier: MIT */
#include "link-client.h"
#include "src/rom/rom.h"
#include <string.h>
int legacy_main(int, char **);

/* Binary is the default. Legacy access requires an explicit transitional prefix. */
int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "rom-info")) return rom_info_cli();
    if (argc > 1 && !strcmp(argv[1], "legacy")) return legacy_main(argc - 1, argv + 1);
    if (argc > 1 && (!strcmp(argv[1], "usb") || !strcmp(argv[1], "inspect") || !strcmp(argv[1], "gpt")))
        return legacy_main(argc, argv);
    return link_cli(argc, argv);
}
