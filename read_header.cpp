// приложение читает заголовок физионета 
// и конвертирует в свой формат hdr

#include "def.h"

int main(int argc, char *argv[])
{
    char *filename;

    if (argc > 1)
        filename = argv[1];
    else
        return 1; // ошибка - неправильное кол-во аргументов

    // после создания ссылки ее нельзя привязать к другому объекту,
    // поэтому привяжем через указатель
    ostream* this_out = &cout;
    ofstream outfile;
    if (argc > 2)
    {
        outfile.open(argv[2], ios::out | ios::binary);
        this_out = &outfile;
    }
    ostream &out = *this_out;

    // ??? входные аргументы
    // int nChannel = 0;
    //int nFirstPnt = 0;
    //int nPoints = INT32_MAX / 2;
    const int c_nPadLengthSec = 60;

#ifndef _DEBUG
    wfdbquiet(); // чтобы не выдавал сообщения об ошибках!
#endif

    wfdb_addtopath(filename);

    fs::path p_ = filename;
    string extname = p_.extension().string(); // _extensionName(filename); //
    char *c_extname = const_cast<char *>(extname.c_str() + 1);
    string pathname = p_.replace_extension("").string(); // _pathName(filename); //
    char *c_pathname = const_cast<char *>(pathname.c_str());
    string recname = p_.stem().string();  // _recordName(filename); //
    char *c_recname = const_cast<char *>(recname.c_str());

    struct stat attrib;         // create a file attribute structure
    stat(filename, &attrib);     // get the attributes of afile.txt

    struct tm* clock = gmtime(&(attrib.st_mtime)); // Get the last modified time and put it into the time structure
    char t_buf[sizeof("2011-10-08T07:07:09Z")];
    strftime(t_buf, sizeof(t_buf), "%FT%T", clock); //const char* t = asctime(clock);

    int nsig = isigopen(c_recname, 0, 0);
    if (!nsig)
        return 1;
    vector<WFDB_Siginfo> vsinfo(nsig);

    isigopen(c_recname, &vsinfo[0], nsig);

    double freq = (double)sampfreq(c_recname);
    long pnt = vsinfo.front().nsamp;
    vector<WFDB_Sample> vSmp(nsig);
    int nPadLen = 0; //(int)(c_nPadLengthSec * freq);

    const char* s = timstr(WFDB_Time(0));

    double div = 1000;
    double amp = (double)(vsinfo[0].gain == 0 ? WFDB_DEFGAIN : vsinfo[0].gain);
    double lsb_ = div / amp; // ??? проверить

    out << nsig << " " << freq << " " << lsb_ << endl;
    out << 0 << "\t" << pnt << " " << t_buf << endl;
    for (int ch = 0; ch < nsig; ch++) {
        char * chanme = vsinfo[ch].desc;
        out << chanme << "\t";
    }
    out << endl;
    for (int ch = 0; ch < nsig; ch++) {
        double div = 1000;
        double amp = (double)(vsinfo[ch].gain == 0 ? WFDB_DEFGAIN : vsinfo[ch].gain);
        double lsb = div / amp; // ??? проверить
        out << lsb << "\t"; 
    }
    out << endl;
    for (int ch = 0; ch < nsig; ch++) {
        const char* units = vsinfo[ch].units == 0 ? "mV" : vsinfo[ch].units; // это фактически неверно, т.к. мы пишем микровольты
        out << units << "\t"; 
    }
    out << endl;

    isigclose();
    wfdbquit();

    if (outfile.is_open())
        outfile.close(); // все равно вызовется в декструкторе

    return 0;
}