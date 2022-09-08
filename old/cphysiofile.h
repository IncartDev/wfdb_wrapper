#pragma once

#include "cphysiofile_EXPORTS.h"

#include <string>

extern "C" CPHYSIOFILE_EXPORT void f();
extern "C" CPHYSIOFILE_EXPORT int get2 ();
extern "C" CPHYSIOFILE_EXPORT double sumMyArgs (float i, float j);
extern "C" CPHYSIOFILE_EXPORT void showWchars (wchar_t* s);


extern "C" struct CPHYSIOFILE_EXPORT SomeData {
    signed int x;
    double y;
    // std::string z;
};

extern "C" CPHYSIOFILE_EXPORT SomeData getSomeData(int x, double y); //, std::string z);

// string _recordName(const std::string& filepath);
// string _extensionName(const std::string& filepath);
// string _pathName(const std::string& filepath);

// extern "C" CPHYSIOFILE_EXPORT bool openAnnotation(char* filename, bool for_write = false);

// extern "C" CPHYSIOFILE_EXPORT void closeAnnotation();

extern "C" CPHYSIOFILE_EXPORT const char* readAnnotation(char* filename);

extern "C" CPHYSIOFILE_EXPORT void foo();