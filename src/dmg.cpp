#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>     // DEBUG

#include "cpu.h"
#include "mmu.h"


int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: dmg <BOOT ROM> <GAME ROM>\n");
        return EXIT_FAILURE;
    }

    MMU mmu;
    CPU cpu(&mmu);

    if (!mmu.load(argv[1], 0x0000)) {
        return EXIT_FAILURE;
    }

    bool running = true;
    while (running) {
        cpu.step();

        // DEBUG
        sleep(1);
    }

    return EXIT_SUCCESS;
}
