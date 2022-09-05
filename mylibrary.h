#pragma once

#include "mylibrary_EXPORTS.h"

#include <string>


extern "C" MYLIBRARY_EXPORT void f();
extern "C" MYLIBRARY_EXPORT int get2 ();
extern "C" MYLIBRARY_EXPORT double sumMyArgs (float i, float j);

extern "C" struct MYLIBRARY_EXPORT SomeData {
    signed int x;
    double y;
    // std::string z;
};

extern "C" MYLIBRARY_EXPORT SomeData getSomeData(int x, double y); //, std::string z);