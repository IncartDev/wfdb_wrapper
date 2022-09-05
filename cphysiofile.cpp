#include "cphysiofile.h"
#include <iostream>

void f() {
  std::cout << "f()\n";
}

// X::X() {
//   std::cout << "X::X()\n";
// }

// void X::mX() {
//   std::cout << "X::mX()\n";
// }

SomeData getSomeData(int x, double y) //, std::string z)
{
  SomeData data {x + 1, y + 1}; //, z};
  return data;
}

int get2 (){
 return 2;
}

double sumMyArgs (float i, float j){
 return i+j;
}