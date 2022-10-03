// приложение читает сигналы из файла формата физионет
// и выводит их в стандартный вывод в бинарном формате

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

    // const int c_nPadLengthSec = 0; // без дописывания в хвост
    // const int c_nPadLengthSec = 60; // минута наращивалась в аналайзере
    const int c_nPadLengthSec = 10; // 8-10 секунд наращивалось в матлабе - для микробокса

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

    int nsig = isigopen(c_recname, 0, 0);
    if (!nsig)
        return 1;
    vector<WFDB_Siginfo> vsinfo(nsig);

    isigopen(c_recname, &vsinfo[0], nsig);
    // if (nChannel < 0 || nChannel >= nsig)
    // {
    //     return 1;
    // }

    double freq = (double)sampfreq(c_recname);
    long pnt = vsinfo.front().nsamp;
    vector<WFDB_Sample> vSmp(nsig);
    int nPadLen = (int)(c_nPadLengthSec * freq);

    // vector<vector<int>> ppnt(nsig);
    // for (int ch = 0; ch < nsig; ch++)
    // {
    //     ppnt[ch] = vector<int>(nPoints);
    // }

    //	fill in the points with correct indices
    int nStart = 0; //max(0, nFirstPnt);
    int nEnd = pnt; //min(pnt, nFirstPnt + nPoints);
    if (nEnd > nStart)
    {
        // почему-то, если сначала не установить время на последнюю точку, то потом ее нельзя прочитать...
        if (isigsettime(pnt-1) < 0 || getvec(&vSmp[0]) < 0)
        {
            isigclose();
            return 1;
        }
        // устанавливаем на первую точку
        if (isigsettime(nStart) < 0)
        {
            isigclose();
            return 1;
        }
        // cout << "----------------------" << endl;
        // cout << nsig << " * " << sizeof(WFDB_Sample) << endl;
        for (int i = nStart; i < nEnd; i++)
        {
            //isigsettime(i);
            if (getvec(&vSmp[0]) < 0)
            {
                isigclose();
                return 1;
            }
            out.write((const char *)&vSmp[0], nsig * sizeof(WFDB_Sample));
            // log_arr((int*)&vSmp[0], nsig);
        }
    }

    // pad the points behind the file end

    int nPadStart = pnt; //max(pnt, nFirstPnt);
    int nPadEnd = pnt + nPadLen; // min(nFirstPnt + nPoints, pnt + nPadLen);
    if (nPadEnd > nPadStart)
    {
        // уже прочитали последнее значение из файла - размножим его
        for (int i = nPadStart; i < nPadEnd; i++)
        {
            out.write((const char *)&vSmp[0], nsig * sizeof(WFDB_Sample)); // пишем все точки в стандартный вывод в бинарном виде
        }
    }

    isigclose();
    wfdbquit();
    
    if (outfile.is_open())
        outfile.close(); // все равно вызовется в декструкторе

    return 0;
}