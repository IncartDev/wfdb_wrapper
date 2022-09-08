#include "StdAfx.h"
#include <Shlwapi.h>

#ifdef NETVER
#include "ForFile/InfoMap.h"
#include "ForFile/FileDlls.h"
#include "ForFile/LoadFileDlls.h"
#include "ForFile/ECGTempl.h"
#include "ForFile/IniFile.h"
#else
#include "../ForFile/InfoMap.h"
#include "../ForFile/FileDlls.h"
#include "../ForFile/LoadFileDlls.h"
#include "../ForFile/ECGTempl.h"
#include "../ForFile/IniFile.h"
#endif

#include <vector>
using std::vector;
static int error_print;
#include <string>
using std::string;

char *RecordName(const CStringA &strECGFileName)
{
    //	get record name - no path, no extension
    static CStringA record;
    record = strECGFileName.Mid(strECGFileName.ReverseFind('\\') + 1);
    record = record.Left(record.ReverseFind('.'));
    return (LPTSTR)record.GetBuffer();
}

char *PathName(const CStringA &strECGFileName)
{
  static CStringA record;
  record = strECGFileName.Left(strECGFileName.ReverseFind('.'));
  return (LPTSTR)record.GetBuffer();
}

char *ExtensionName(const CStringA &strECGFileName)
{
  static CStringA Extension;
  Extension = strECGFileName.Mid(strECGFileName.ReverseFind('.')+1);
  return (LPTSTR)Extension.GetBuffer();
}


CStringA GetShortFileName(const CStringW &strECGFileName)
{
  CStringW ShortFile;
  ShortFile.Truncate( GetShortPathNameW((LPCWSTR)strECGFileName,(LPWSTR)ShortFile.GetBufferSetLength(MAX_PATH),MAX_PATH));
  return CStringA(ShortFile);
}
//---------------------------------------------------------------------------------------------------------------------------------------
extern "C" __declspec( dllexport ) void DelClassSave( void *pClassSave )				//Delete ClassSave
{

}
//---------------------------------------------------------------------------------------------------------------------------------------
extern "C" __declspec( dllexport ) LPCTSTR Get_FileExt()
{
    return _T("dat");
};

extern "C" __declspec(dllexport) void CloseFile(void* pClassSave)		//Close File
{
  wfdbquit();
}

//---------------------------------------------------------------------------------------------------------------------------------------
extern "C" __declspec( dllexport ) int TryFileType( const CStringW &strECGFileName )//Test File for this type 
{
  CStringA ECGFile = GetShortFileName(strECGFileName);
  CStringW strHea = ECGFile;
  strHea = strHea.MakeLower();
#ifdef NETVER
  if (!strHea.Replace(L".dat", L".hea"))
#else
  if (!strHea.Replace(_T(".dat"), _T(".hea")))
#endif
    return RET_UNKNOWNTYPE;

  if (_PathFileExists(strHea) == 0)
    return RET_UNKNOWNTYPE;

  wfdbquit();
  if (ECGFile.Find('\\') == -1 && ECGFile.Find('/') == -1)
  {
    char a[MAX_PATH];
    DWORD l = GetCurrentDirectory(MAX_PATH, a);
    a[l] = '\\'; a[l + 1] = 0;
    wfdb_addtopath(a);    
  }
  else
    wfdb_addtopath((LPTSTR)ECGFile.GetBuffer());

  int SaveError = error_print;
  error_print = 0;
  CStringA record = ECGFile.Mid(ECGFile.ReverseFind('\\') + 1);
  record = record.Left(record.ReverseFind('.'));
  char* Record = record.GetBuffer();
  if (wfdb_open("hea", Record, WFDB_READ) == NULL || isigopen(Record, 0, 0) <= 0)
  {
    error_print = SaveError;
    return RET_UNKNOWNTYPE;
  }
  error_print = SaveError;
  return RET_OK ;
};

const int c_nPadLengthSec = 60;

//---------------------------------------------------------------------------------------------------------------------------------------
extern "C" __declspec( dllexport ) int Parse_Head(const CStringW &strECGFileName, int nFool, 
                                                  CInfoMap *pInfoMap, CString &strECGFileType)	
                                                  //Load Head
{
//	strECGFileType = _T("?");
//	pInfoMap->SetTextInfo(_T("fname"), strECGFileName, FALSE);
//	pInfoMap->SetLongInfo(_T("rtNum"), 23);
//	pInfoMap->SetLongInfo(_T("monTyp"), 23);

    //	add file path to db path
    CStringA ECGFile = GetShortFileName(strECGFileName);

    wfdbquit();
    if (ECGFile.Find('\\') == -1 && ECGFile.Find('/') == -1)
    {
      char a[MAX_PATH];
      DWORD l = GetCurrentDirectory(MAX_PATH, a);
      a[l] = '\\'; a[l + 1] = 0;
      wfdb_addtopath(a);    
    }
    else
      wfdb_addtopath((LPTSTR) (LPCTSTR) (ECGFile));

    //	obvious parameters
    pInfoMap->SetTextInfo(_T("NameFlDll"), __L("FilePhysionet"), FALSE);

    CStringA record = ECGFile.Mid(ECGFile.ReverseFind('\\') + 1);
    record = record.Left(record.ReverseFind('.'));
    char* Record = record.GetBuffer();

    long freq = (long)sampfreq(Record);
    pInfoMap->SetLongInfo(_T("Freq"), freq);

    //	start is file creation time
    CFileStatus status;
    CFile::GetStatus(ECGFile, status);
    SYSTEMTIME stime;
    status.m_ctime.GetAsSystemTime(stime);
    stime.wHour = stime.wMinute = stime.wSecond = 0;
    unsigned int iHour, iMin, iSec;
    if (sscanf(timstr(0), "[%u:%u:%u]", &iHour, &iMin, &iSec) == 3)
    {
      stime.wHour = iHour;  stime.wMinute = iMin;  stime.wSecond = iSec;
    }
    pInfoMap->SetTimeInfo(_T("Start"), COleDateTime(stime));	//  Дата съема

    //	open signal file
    int nsig = isigopen(Record, 0, 0);
    if (nsig <= 0)
        return RET_BADFILE;
    vector<WFDB_Siginfo> vsinfo(nsig);
    isigopen(Record, &vsinfo[0], nsig);

    pInfoMap->SetLongInfo(_T("TotCnl"), nsig);
    pInfoMap->SetLongInfo(_T("TotECG"), nsig);
    pInfoMap->SetLongInfo(_T("TotPnt"), vsinfo.front().nsamp + c_nPadLengthSec * freq);

    for (int i = 0; i < nsig; i++)
    {
        CStringW desc = CStringW(vsinfo[i].desc);
        if (desc == L"ECG")
#ifdef NETVER
            desc.Format(L"ECG%d", i);
#else
            desc.Format(_T("ECG%d"), i);
#endif

        pInfoMap->SetTextInfo(_T("Ch%u.Des"), desc, FALSE, i);
        pInfoMap->SetTextInfo(_T("Ch%u.Tag"), desc, FALSE, i);
        pInfoMap->SetTextInfo(_T("Ch%u.Lab"), vsinfo[i].units == 0 ? L"mV" : CStringW(vsinfo[i].units), FALSE, i);
        pInfoMap->SetLongInfo(_T("Ch%u.Div"), 1000, i);
        pInfoMap->SetLongInfo(_T("Ch%u.Amp"), (LONG)(vsinfo[i].gain == 0 ? WFDB_DEFGAIN : vsinfo[i].gain), i);
        if (vsinfo[i].baseline != 0 && vsinfo[i].gain != 0)
          pInfoMap->SetLongInfo(_T("Ch%u.Nul"), (LONG)(vsinfo[i].baseline), i);
    }
    isigclose();

    CStringA HEAFileName = strECGFileName.Left(strECGFileName.ReverseFind('.'))+".hea";
    try
    {
      CFile HEAFile(HEAFileName,CFile::modeRead | CFile::shareDenyWrite );
      CStringA Text;
      int HeaSize = (int)HEAFile.GetLength();
      HEAFile.Read(Text.GetBufferSetLength(HeaSize), HeaSize);
      int LasPos(0);
      CStringA Line = Text.Tokenize("\n", LasPos);
      while (Line != "")
      { 
        if (Line.Find("<family>") >= 0)
        {
          LPCSTR Sep = ":<>\r";
          LasPos = 0;
          CStringA Part = Line.Tokenize(Sep, LasPos);
          pInfoMap->SetTextInfo(_T("fam"), CStringW(Line.Tokenize(Sep, LasPos).Trim()), FALSE);
          Part = Line.Tokenize(Sep, LasPos);
          pInfoMap->SetTextInfo(_T("nam"), CStringW(Line.Tokenize(Sep, LasPos).Trim()), FALSE);
          Part = Line.Tokenize(Sep, LasPos);
          pInfoMap->SetTextInfo(_T("sub"), CStringW(Line.Tokenize(Sep, LasPos).Trim()), FALSE);
          break;
        }
        Line = Text.Tokenize("\n", LasPos);
      }
      HEAFile.Close();
    }
    catch (...)
    {

    }

    return RET_OK;
}
extern "C" __declspec( dllexport ) BOOL Parse_DATA(const CStringW &strECGFileName, CInfoMap *pInfoMap, 
                                                   CQRSInfoArray *pQRSInfoArray, FProgFun pProgFun, 
                                                   void *&pClassSave)			//Load m_MinInfoArray
{
//		if( pInfoMap->GetTimeInfo(_T("StartInf")).GetStatus() != COleDateTime::valid )
//			pInfoMap->SetTimeInfo(_T("StartInf"), StartTime);
//		pInfoMap->SetTimeInfo(_T("Start"),StartTime);	//  Дата съема
//		pInfoMap->SetLongInfo(_T("TotPnt"),UnPack->TotPnt());

    pInfoMap->SetLongInfo("monTyp", 10);	
    //	раньше был: defenchk.h: enum { DEV_TYPE_GENERATOR = 9 }; - этот тип монитора открывается
    //	любой версией программы в любом случае
  //  2011.10.21 по указанию ТВМ и договору с ПАБ присвоен отдельный тип прибора 10, на который нужна своя строчка в файле лицензии

    LONG freq = pInfoMap->GetLongInfo(_T("Freq"));
    LONG pnt = pInfoMap->GetLongInfo(_T("TotPnt"));
    pQRSInfoArray->SetSize(pnt / freq);

//	pQRSInfoArray->SetAtGrow(0,
//		CQRSInfo(freq, 1, CQRSInfo::cnBad | CQRSInfo::cnBadRR, 0, CQRSInfo::c_Def, 0, 0, 0)
//		);
    for (int i = 0; i < pnt / freq; i++)
        pQRSInfoArray->SetAtGrow(i,
            CQRSInfo(freq, 1, CQRSInfo::cnBad, 0, CQRSInfo::c_Def, 0, 0, 0)
            );


        return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------------------------
extern "C" __declspec( dllexport ) BOOL Load_DATA(const CStringW &strECGFileName, DWORD* ppnt, 
                                                  int nChannel, int nFirstPnt, int nPoints, 
                                                  CQRSInfoArray * /*pQRSInfoArray*/, void *pClassSave)
                                                  //Load Data From File
{
    //	Open file, check number of channels
    CStringA ECGFile = GetShortFileName(strECGFileName);
    wfdb_addtopath((LPTSTR)(LPCTSTR)(ECGFile));
    CStringA record = ECGFile.Mid(ECGFile.ReverseFind('\\') + 1);
    record = record.Left(record.ReverseFind('.'));
    char* Record = record.GetBuffer();
    int nsig = isigopen(Record, 0, 0);
    if (!nsig)
        return FALSE;
    vector<WFDB_Siginfo> vsinfo(nsig);


    isigopen(Record, &vsinfo[0], nsig);
    if (nChannel < 0 || nChannel >= nsig)
    {
      return FALSE;
    }

    long freq = (long)sampfreq(Record);
    LONG pnt = vsinfo.front().nsamp;
    vector<WFDB_Sample> vSmp(nsig);

    //	pad the points behind the file end
    int nPadStart = max(pnt, nFirstPnt);
    int nPadEnd = min(nFirstPnt + nPoints, pnt + c_nPadLengthSec * freq);
    if (nPadEnd > nPadStart)
    {
        //	get last value of signal to use for padding
        if (isigsettime(pnt-1) < 0 || getvec(&vSmp[0]) < 0)
        {
          return FALSE;
        }
        DWORD PadValue = vSmp[nChannel];
        //	do it
        for (int i = nPadStart; i < nPadEnd; i++)
            ppnt[i - nFirstPnt] = PadValue;
    }

    //	fill in the points with correct indices
    int nStart = max(0, nFirstPnt);
    int nEnd = min(pnt, nFirstPnt + nPoints);
    if (nEnd > nStart)
    {
        //	Read data
        if (isigsettime(nStart) < 0)
        {
          return FALSE;
        }
        for (int i = nStart; i < nEnd; i++)
        {
            if (getvec(&vSmp[0]) < 0)
            {
              return FALSE;
            }
            ppnt[i - nFirstPnt] = vSmp[nChannel];
        }
    }

    isigclose();
    //wfdbquit();
    return TRUE;
}

extern "C" __declspec( dllexport ) bool OpenAnnotation(const wchar_t *ECGFileName, bool bForWriting)
{
  CStringA strECGFileName = GetShortFileName(ECGFileName);
  if(strECGFileName.GetLength() == 0)
  {
    CStringW t = ECGFileName;
    strECGFileName = GetShortFileName(t.Left(t.ReverseFind('\\')));
    t = t.Mid(t.ReverseFind('\\')+1);

    strECGFileName = strECGFileName+"\\"+CStringA(t); 
  }
  strECGFileName.Replace("/", "\\");

  wfdb_addtopath((LPTSTR) (LPCTSTR) strECGFileName);
  WFDB_Anninfo OpenInfo;                         
  OpenInfo.name = ExtensionName(strECGFileName);
  OpenInfo.stat = bForWriting ? WFDB_WRITE : WFDB_READ;
  if(bForWriting)
    return annopen(PathName(strECGFileName), &OpenInfo, 1) == 0;
  else
    return annopen(RecordName(strECGFileName),&OpenInfo,1) == 0;
}

extern "C" __declspec( dllexport ) void CloseAnnotation()
{
  wfdb_oaflush();
  wfdbquit();
}

extern "C" __declspec(dllexport) BOOL WriteAnnotation(LONG Point, unsigned char Type, signed char SubType, signed char Channel, const char *AddInfo, signed char Num)
{
  WFDB_Annotation Annotation;
  Annotation.time = Point;
  Annotation.anntyp = Type;
  Annotation.subtyp = SubType;
  Annotation.chan = Channel;
  Annotation.num = Num;
  std::string s;
  if (AddInfo)
  {
      s = AddInfo;
      char ch = char(s.length());
      s.insert(0, 1, ch);
      AddInfo = s.c_str();

      Annotation.aux = reinterpret_cast<unsigned char*>(const_cast< char *>(AddInfo));
  }
  else
    Annotation.aux = 0;
  int ret = putann(0,&Annotation);
  return ret == 0;
}

extern "C" __declspec( dllexport ) bool ReadAnnotation(LONG* Point,unsigned char* Type, signed char* SubType, signed char* Channel, 
                                                       string *AddInfo)
{
  WFDB_Annotation Annotation;
  if (getann(0,&Annotation) == 0)
  {
    if (Point)
      *Point = Annotation.time;
    if (Type)
      *Type = Annotation.anntyp;
    if (SubType)
      *SubType = Annotation.subtyp;
    if (Channel)
      *Channel = Annotation.chan;
    if (AddInfo && Annotation.aux != 0)
        *AddInfo = reinterpret_cast<const char*>(Annotation.aux);
    return true;
  }
  else
    return false;
}

extern "C" __declspec( dllexport ) void RewindAnnotation()
{
  iannsettime(0);
}
