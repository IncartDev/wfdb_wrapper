#include <cstring>
#include <locale>
#include <iostream>
#include <cstring>

#include "wfdblib/wfdblib.h"
#include "wfdblib/wfdb.h"

using namespace std;

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

int main(int argc, char* argv[]) {

    // char* filename = "I:\\code\\wfdb_wrapper\\data\\I22.atr";
    char* filename;
    
    if (argc > 1)
        filename = argv[1];
    else
        return 1; // ошибка - неправильное кол-во аргументов

    wfdb_addtopath(filename);
    WFDB_Anninfo OpenInfo;

    string extname = _extensionName(filename);
    OpenInfo.name = const_cast<char*>(extname.c_str()); //&*str.begin() - другой способ получить char*
    OpenInfo.stat = WFDB_READ;

    string pathname = _pathName(filename);
    string recname = _recordName(filename);

    if (annopen(const_cast<char*>(recname.c_str()), &OpenInfo,1) != 0)
        return 1; // ошибка - не удалось открыть файл

    double freq = (double)sampfreq(const_cast<char*>(recname.c_str()));
    std::cout << "#freq:" << freq << std::endl;

    iannsettime(0); // rewind

    WFDB_Annotation Annotation;
    while (getann(0, &Annotation) == 0)
    {
        std::cout << Annotation.time;
        char* symbol = annstr(Annotation.anntyp);
        std::cout << ' ' << symbol << ' ';
        if (Annotation.aux != 0)
            std::cout << Annotation.aux;
        std::cout << std::endl;
    }

    return 0;
}