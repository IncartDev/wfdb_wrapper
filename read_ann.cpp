// приложение читает разметку из файла формата физионет
// и выводит ее в стандартный вывод в текстовом формате (csv)

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

    WFDB_Anninfo OpenInfo;
    OpenInfo.name = c_extname; //&*str.begin() - другой способ получить char*
    OpenInfo.stat = WFDB_READ;

    if (annopen(c_recname, &OpenInfo, 1) != 0)
        return 1; // ошибка - не удалось открыть файл

    double freq = (double)sampfreq(c_recname);
    out << "#freq:" << freq << endl;

    iannsettime(0); // rewind

    WFDB_Annotation Annotation;
    while (getann(0, &Annotation) == 0)
    {
        out << Annotation.time;
        char *symbol = annstr(Annotation.anntyp);
        out << ' ' << symbol << ' ';
        if (Annotation.aux != 0)
            out << Annotation.aux;
        out << endl;
    }

    wfdbquit();

    if (outfile.is_open())
        outfile.close(); // все равно вызовется в декструкторе
        
    return 0;
}