#pragma once

#include "cphysiofile_EXPORTS.h"

#include <string>


extern "C" CPHYSIOFILE_EXPORT void f();
extern "C" CPHYSIOFILE_EXPORT int get2 ();
extern "C" CPHYSIOFILE_EXPORT double sumMyArgs (float i, float j);

extern "C" struct CPHYSIOFILE_EXPORT SomeData {
    signed int x;
    double y;
    // std::string z;
};

extern "C" CPHYSIOFILE_EXPORT SomeData getSomeData(int x, double y); //, std::string z);