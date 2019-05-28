#include "sound.h"

#include "log.h"


Sound::Sound() : mmu(nullptr)
{

}


Sound::~Sound()
{

}


bool Sound::init()
{
    if (mmu == nullptr) {
        error("No MMU linked to Sound\n");
        return false;
    }

    return true;
}


void Sound::reset()
{
    // TODO
}


void Sound::update()
{
    // TODO
}


void Sound::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


void Sound::serialize(std::ofstream &/*file*/)
{
    // TODO
}


void Sound::deserialize(std::ifstream &/*file*/)
{
    // TODO
}

