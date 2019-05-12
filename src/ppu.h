#ifndef PPU_H
#define PPU_H

#include "mmu.h"

/**
 * @brief      Pixel Processing Unit
 */
class PPU {
public:
    PPU(MMU *mmu);

    bool init();
    bool draw();

private:
    MMU *mmu;
};

#endif /* PPU_H */
