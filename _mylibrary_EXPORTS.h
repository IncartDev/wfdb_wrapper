#pragma once

#ifdef _WIN32
    #ifdef mylibrary_EXPORTS
        #define MYLIBRARY_EXPORT __declspec(dllexport)
    #else
        #define MYLIBRARY_EXPORT __declspec(dllimport)
    #endif
#else
    #define MYLIBRARY_EXPORT
#endif