#include "cphysiofile.h"
#include <iostream>
#include <sstream>

#include "wfdblib/wfdblib.h"
#include "wfdblib/wfdb.h"

using namespace std;

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

void showWchars (wchar_t* s){
  size_t len = wcslen(s);
  std::wcout << "длина" << len << "\n";
  std::wcout << "строка" << s << "\n";
  std::wcout << s << "\n";
}

std::string _recordName(const std::string& filepath)
{
  size_t ibeg = filepath.find_last_of('\\');
  std::string filename = filepath.substr(ibeg+1);
  ibeg = filename.find_last_of('.');
  string recname = filename.substr(0, ibeg);
  return recname;
}
std::string _extensionName(const std::string& filepath)
{
  size_t ibeg = filepath.find_last_of('.');
  std::string ext = filepath.substr(ibeg+1);
  return ext;
}
std::string _pathName(const std::string& filepath)
{
  size_t ibeg = filepath.find_last_of('.');
  string pathname = filepath.substr(0, ibeg);
  return pathname;
}

bool openAnnotation(char* filename, bool for_write) {
  string s_filename(filename);
  
  wfdb_addtopath(filename);
  WFDB_Anninfo OpenInfo;

  string extname = _extensionName(filename);
  OpenInfo.name = const_cast<char*>(extname.c_str()); //&*str.begin() - другой способ получить char*

  OpenInfo.stat = for_write ? WFDB_WRITE : WFDB_READ;
  string pathname = _pathName(filename);
  string recname = _recordName(filename);
  // long freq = (long)sampfreq(const_cast<char*>(recname.c_str()));
  // std::cout << freq;

  if(for_write)
  {
    return annopen(const_cast<char*>(pathname.c_str()), &OpenInfo, 1) == 0;
  }
  else
  {
    return annopen(const_cast<char*>(recname.c_str()), &OpenInfo,1) == 0;
  }
}

void closeAnnotation() {
  wfdb_oaflush();
  wfdbquit();
}

// чтение меток в текстовую строку
const char* readAnnotation(char* filename)
{
  std::ostringstream stream;

  openAnnotation(filename, false);

  string recname = _recordName(filename);
  double freq = (double)sampfreq(const_cast<char*>(recname.c_str()));
  stream << "#freq:" << freq << std::endl;

  iannsettime(0); // rewind

  WFDB_Annotation Annotation;
  while (getann(0, &Annotation) == 0)
  {
    stream << Annotation.time;
    char* symbol = annstr(Annotation.anntyp);
    stream << ' ' << symbol;
    if (Annotation.aux != 0)
      stream << ' ' << Annotation.aux;
    else
      stream << ' ';
    stream << std::endl;
  }

  return stream.str().c_str();
}

extern "C" bool foo(iostream& io)
{
  io << "hello!";
  return true;
}

// запись аннотации в формате Physionet (надо ли?)
void writeAnnotation()
{
  
}