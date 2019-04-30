#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "log.h"

#include "dmg.h"


int DMG::run(const char *path_bios, const char *path_rom)
{
    cpu = new CPU(&mmu);
    //gpu = new GPU(&mmu);

    cpu->reset();

    if (!mmu.load(path_bios, 0x0000)) {
        return EXIT_FAILURE;
    }

    size_t frame_count = 0;

    bool running = true;
    while (running) {
        step_frame();

        //gpu.draw();
    }

    return EXIT_SUCCESS;
}


/**
 * @brief      Give time to CPU while no frame should be draw
 * @return     true when a frame should be rendered
 */
bool DMG::step_frame()
{
    while (cpu->clock < FRAME_CYCLES) {
        cpu->step();
    }

    cpu->clock -= FRAME_CYCLES;

    return true;
}
