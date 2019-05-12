#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "log.h"
#include "dmg.h"

#include "main.h"


int main(int argc, char *argv[])
{
    info("DMG emulation\n");

    if (argc != 3) {
        error("Usage: dmg <BOOT ROM> <GAME ROM>\n");
        return EXIT_FAILURE;
    }

    info("BIOS: %s\n", argv[1]);
    info("ROM: %s\n", argv[2]);

    DMG dmg;
    dmg.run(argv[1], argv[2]);

    return EXIT_SUCCESS;
}
