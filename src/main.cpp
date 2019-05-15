#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "log.h"
#include "dmg.h"

#include "main.h"


int main(int argc, char *argv[])
{
    info("DMG emulation\n");

    if (argc < 3 || argc > 4) {
        error("Usage: dmg <BOOT ROM> <GAME ROM> [<PALETTE>]\n");
        return EXIT_FAILURE;
    }

    info("BIOS: %s\n", argv[1]);
    info("ROM: %s\n", argv[2]);

    DMG dmg;
    if (!dmg.init(argv[1], argv[2])) {
        return EXIT_FAILURE;
    }

    if (argc == 4) {
        dmg.set_palette(argv[3][0]);
    }

    return dmg.run();
}
