#ifndef SOUND_H
#define SOUND_H

#include "defines.h"
#include "mmu.h"


/**
 * @brief      Hanldes sounds
 */
class Sound {
    MMU *mmu;

public:
    Sound();
    ~Sound();

    bool init();
    void reset();
    void update();

    void set_mmu(MMU *mmu);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);
};

#endif /* SOUND_H */
