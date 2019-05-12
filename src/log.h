#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <SDL2/SDL.h>


#ifdef DEBUG
    #define debug(fmt, ...) SDL_LogDebug(SDL_LOG_CATEGORY_TEST, fmt, ## __VA_ARGS__)
#else
    #define debug(fmt, ...)
#endif

#ifdef TEST
    #define info(fmt, ...)
    #define error(fmt, ...)
#else
    #define info(fmt, ...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, fmt, ## __VA_ARGS__)
    #define error(fmt, ...) SDL_LogError(SDL_LOG_CATEGORY_ERROR, fmt, ## __VA_ARGS__)
#endif

#endif /* LOG_H */
