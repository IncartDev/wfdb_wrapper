#include "cphysiofile.h"
#include <cstring>
#include <locale>
#include <iostream>
#include <cstring>

int main() {
  // test library
  //f();
  // X x;
  // x.mX();
  // std::wcout << "User-preferred locale setting is " << std::locale("").name().c_str() << '\n';
  // std::locale::global(std::locale(""));
  //wchar_t* hi = L"Привет";
  //showWchars(hi);
  // 

  char* file = "I:\\code\\wfdb_wrapper\\data\\I22.atr";
  
  std::string out = readAnnotation(file);
  // std::cout << Point << " " << Type << " " << SubType << " " << Channel << " " << AddInfo << std::endl;
  std::cout << out;
}