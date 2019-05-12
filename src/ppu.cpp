#include "ppu.h"

#include <SDL/SDL.h>

PPU::PPU(MMU *mmu) : mmu(mmu)
{

}

bool PPU::init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_WM_SetCaption("DMG - Emulator", NULL);
    SDL_SetVideoMode(160, 144, 32, SDL_HWSURFACE);

    return true;
}

bool PPU::draw()
{
    return false;
}
