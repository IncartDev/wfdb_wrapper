#include "StdAfx.h"


#include "PhysionetAnnotationFile.h"
#include "Export/Physionet.h"


extern "C" __declspec( dllexport ) BOOL WriteAnnotation(LONG Point,unsigned char Type, 
                                                        signed char SubType, signed char Channel, const char *AddInfo, signed char Num);
extern "C" __declspec( dllexport ) bool ReadAnnotation(LONG* Point,unsigned char* Type, signed char* SubType, 
                                                       signed char* Channel, string *AddInfo);
extern "C" __declspec( dllexport ) void RewindAnnotation();

extern "C" __declspec( dllexport ) bool OpenAnnotation(const wchar_t *ECGFileName, bool bForWriting);
extern "C" __declspec( dllexport ) void CloseAnnotation();



CPhysionetAnnotationFile::~CPhysionetAnnotationFile()
{
    if(m_FileName)
        delete [] m_FileName;
}

void CPhysionetAnnotationFile::Open(const wchar_t *FileName, bool bForWriting)
{
    long len = wcslen(FileName);
    m_FileName = new wchar_t[len+1];
    memset(m_FileName,0,(len+1)*sizeof(wchar_t));
    memcpy(m_FileName,FileName,len*sizeof(wchar_t));

    if (!OpenAnnotation(FileName, bForWriting))
    throw "Не могу открыть указанный файл аннотации physionet";
}

BOOL CPhysionetAnnotationFile::Write(LONG Point, unsigned char Type, signed char SubType, signed char Channel, const char *AddInfo, signed char Num)
{
    return WriteAnnotation(Point, Type, SubType, Channel, AddInfo, Num);
}

bool CPhysionetAnnotationFile::Read(long *Point,unsigned char *Type, signed char *SubType, signed char *Channel, string *AddInfo)
{
  return ReadAnnotation(Point, Type, SubType, Channel, AddInfo);
}

extern "C" void sortann(char* record, char* ann);
CStringA GetShortFileName(const CStringW& strECGFileName);
char* RecordName(const CStringA& strECGFileName);
char* PathName(const CStringA& strECGFileName);
char* ExtensionName(const CStringA& strECGFileName);

void SortAnnot(const wchar_t* FileName)
{
    CStringA strECGFileName = GetShortFileName(FileName);
    if (strECGFileName.GetLength() == 0)
    {
        CStringW t = FileName;
        strECGFileName = GetShortFileName(t.Left(t.ReverseFind('\\')));
        t = t.Mid(t.ReverseFind('\\') + 1);
        strECGFileName = strECGFileName + "\\" + CStringA(t);
    }
    strECGFileName.Replace("/", "\\");

    wfdb_addtopath((LPTSTR)(LPCTSTR)strECGFileName);

    int iP = strECGFileName.ReverseFind(_T('\\'));
    CString strPath = strECGFileName.Left(iP + 1);

    char lpszCurDir[MAX_PATH + 1];
    ::GetCurrentDirectory(MAX_PATH, lpszCurDir);
    ::SetCurrentDirectory(strPath);

    sortann(RecordName(strECGFileName), ExtensionName(strECGFileName));

    ::SetCurrentDirectory(lpszCurDir);
}
void CPhysionetAnnotationFile::Close()
{
    wfdbquiet();
    CloseAnnotation();
    SortAnnot(m_FileName);
}

void CPhysionetAnnotationFile::Rewind()
{
  RewindAnnotation();
}
const wchar_t * CPhysionetAnnotationFile::GetFileName()
{
    return m_FileName;
}
