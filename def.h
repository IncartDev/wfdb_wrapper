#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <filesystem>

#include "wfdblib/wfdblib.h"
#include "wfdblib/wfdb.h"

#include <sys/stat.h>
//#include <unistd.h>
#include <time.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#ifdef _WIN32
#define FILE_SEP '\\'
#else
#define FILE_SEP '/'
#endif

using namespace std;
namespace fs = std::filesystem;

std::string _recordName(const std::string &filepath)
{
    size_t ibeg = filepath.find_last_of(FILE_SEP);
    std::string filename = filepath.substr(ibeg + 1);
    ibeg = filename.find_last_of('.');
    string recname = filename.substr(0, ibeg);
    return recname;
}
std::string _extensionName(const std::string &filepath)
{
    size_t ibeg = filepath.find_last_of('.');
    std::string ext = filepath.substr(ibeg + 1);
    return ext;
}
std::string _pathName(const std::string &filepath)
{
    size_t ibeg = filepath.find_last_of('.');
    string pathname = filepath.substr(0, ibeg);
    return pathname;
}

void log_arr(int* arr, int len)
{
    for (int i = 0; i < len; i++)
        cout << arr[i] << " ";
    cout << endl;
}