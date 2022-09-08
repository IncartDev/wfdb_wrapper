#include "StdAfx.h"
#undef MessageBox
#undef SetCurrentDirectory

#include "WFDBlib/wfdb.h"

#include "ComInterfaces\IKTRawData.h" 
#include "ComInterfaces\IKTQTSearch.h" 
#include "ComInterfaces\IKTQRSSearch.h" 
#include "ComInterfaces\IKTArrhythmia.h" 
#include "ComInterfaces\IKTLSBFilter.h"
#include "Export/Export.h"


#include <vector>
#include <algorithm>
#include <iostream>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::IO;

float GetLSB(IKTRawData *RawData, int Channel)
{
    LONG lDiv, lAmp;
    RawData->GetLongInfo(L"Ch%u.Div", &lDiv, Channel, 0);
    RawData->GetLongInfo(L"Ch%u.Amp", &lAmp, Channel, 0);
    return (float)lDiv / lAmp;
}

bool ExportFromArrhythmia(IKTCompLinker *Linker, EExportErr *pErr, BSTR FileName, BSTR Library, LONG StartPos, LONG EndPos)
{
    HRESULT hr;

    CComPtr<IKTArrhythmia> Arrhythmia(0);
    VARIANT_BOOL Found;
    Linker->CheckComponentInstance(L"Arrhythmia", &Found);

    if (Found == VARIANT_TRUE && SUCCEEDED(Linker->ExistsComponentInterface(L"Arrhythmia", (IKTCompDispatch**)&Arrhythmia)))
    {
        hr = Arrhythmia->Export(FileName, Library, StartPos, EndPos);
        if (S_OK != hr)
        {
            *pErr = EEE_ExternalModuleErr;
            return false;
        }
        return true;
    }
    else
        return false;
}

bool ExportFromQRSSearch(IKTCompLinker *Linker, EExportErr *pErr, BSTR FileName, BSTR Library, LONG StartPos, LONG EndPos)
{
    HRESULT hr;

    CComPtr<IKTQRSSearch> QRSSearch(0);
    VARIANT_BOOL Found;
    Linker->CheckComponentInstance(L"QRSSearch", &Found);
    if (Found == VARIANT_TRUE && SUCCEEDED(Linker->ExistsComponentInterface(L"QRSSearch", (IKTCompDispatch**)&QRSSearch)))
    {
        hr = QRSSearch->Export(FileName, Library, StartPos, EndPos);
        if (S_OK != hr)
        {
            *pErr = EEE_ExternalModuleErr;
            return false;
        }
    }
    else
    {
        *pErr = EEE_NeedFurtherProcessing;
        return false;
    }
    return true;
}

void ExportQS(IKTCompLinker *Linker, EExportErr *pErr, BSTR FileName, BSTR Library, LONG StartPos, LONG EndPos)
{
    HRESULT hr;

    CComPtr<IKTQRSSearch> QRSSearch(0);
    VARIANT_BOOL Found;
    Linker->CheckComponentInstance(L"QRSSearch", &Found);
    if (Found == VARIANT_TRUE && SUCCEEDED(Linker->ExistsComponentInterface(L"QRSSearch", (IKTCompDispatch**)&QRSSearch)))
    {
        hr = QRSSearch->ExportQS(FileName, Library, StartPos, EndPos);
        if (S_OK != hr)
        {
            *pErr = EEE_ExternalModuleErr;
        }
    }
}

void ExportT(IKTCompLinker *Linker, EExportErr *pErr, BSTR FileName, BSTR Library, LONG StartPos, LONG EndPos)
{
    HRESULT hr;

    CComPtr<IKTQTSearch> QTSearch(0);
    VARIANT_BOOL Found;
    Linker->CheckComponentInstance(L"QTSearch", &Found);
    if (Found == VARIANT_TRUE && SUCCEEDED(Linker->ExistsComponentInterface(L"QTSearch", (IKTCompDispatch**)&QTSearch)))
    {
        hr = QTSearch->Export(FileName, Library, StartPos, EndPos);
        if (S_OK != hr)
        {
            *pErr = EEE_ExternalModuleErr;
        }
    }
}


extern "C" __declspec(dllexport) bool OpenAnnotation(const BSTR ECGFileName, bool bForWriting);
extern "C" __declspec(dllexport) void CloseAnnotation();

extern "C" void sortann(char *record, char *ann);
extern char *RecordName(const CString &strECGFileName);
extern char *ExtensionName(const CString &strECGFileName);

void SortAnnot(CString FileName)
{
    wfdb_addtopath((LPTSTR)(LPCTSTR)FileName);

    int iP = FileName.ReverseFind(_T('\\'));

    CString strPath = FileName.Left(iP + 1);

    char lpszCurDir[MAX_PATH + 1];
    ::GetCurrentDirectory(MAX_PATH, lpszCurDir);
    ::SetCurrentDirectory(strPath);

    sortann(RecordName(FileName), ExtensionName(FileName));

    ::SetCurrentDirectory(lpszCurDir);
}

bool AnnotationsExist(IKTCompLinker *Linker)
{

    BSTR components[] = { L"QRSSearch" };

    VARIANT_BOOL Found;
    for (int i = 0; i < sizeof(components) / sizeof(BSTR); i++)
    {
        Linker->CheckComponentInstance(components[i], &Found);
        if (Found)
            return true;
    }
    return false;
}

extern "C" __declspec(dllexport) EExportErr ExportResults(IKTCompLinker *Linker, IKTProgress *Progress,
    SExportParameters *param, bool bSortAnn, bool bAlreadyOpened, const wchar_t* alg)
{

    if (!AnnotationsExist(Linker))
        return EEE_OK;


    CComBSTR File;
    File.Attach(StringToBSTR(param->m_ResPhysionetFileName));

    try
    {
        try
        {
            //Подготовка параметров
            CComBSTR Library;
            Library.Attach(StringToBSTR(IO::Path::Combine(IO::Path::GetDirectoryName(Application::ExecutablePath), L"FilePhysionet.dll")));


            if (bAlreadyOpened||OpenAnnotation(File, true))
            {
                EExportErr err = EEE_OK;
                //        ExportQS(Linker, &err, &dispparams, &varArgs[dispparams.cArgs]);
                //        ExportT(Linker, &err, &dispparams, &varArgs[dispparams.cArgs]);


                if (wcscmp(alg, L"QRSSearch")==0)
                    ExportFromQRSSearch(Linker, &err, File.m_str, Library.m_str, param->m_StartPos, param->m_EndPos);
                else if (wcscmp(alg, L"Arrhythmia") == 0)
                    ExportFromArrhythmia(Linker, &err, File.m_str, Library.m_str, param->m_StartPos, param->m_EndPos);

                //if (!ExportFromArrhythmia(Linker, &err, File.m_str, Library.m_str, param->m_StartPos, param->m_EndPos)
                /*&& !ExportFromQRSSearch(Linker, &err, File.m_str, Library.m_str, param->m_StartPos, param->m_EndPos)*/
                return err;
            }

        }
        catch (Exception ^)
        {
        }
    }
    __finally
    {
        if (!bAlreadyOpened)
        {
            CloseAnnotation();
            if (bSortAnn)
                SortAnnot(CString(File));
        }
            

        Progress->SetPercent(-1);
        Progress->SetText(L"");
    }
    return EEE_OK;
}

int BlockSize(IKTRawData *RawData)
{
    ULONG Frequency(257);
    RawData->get_Frequency(&Frequency);
    return Frequency * 600;
}

bool FillSiginfo(IKTRawData *RawData, SExportParameters *param,
    WFDB_Siginfo *paSI, char **paFN, char **paDesc)
{
    int curChanInd = 0;
    CComBSTR s;
    s.Attach(StringToBSTR(param->m_ECGPhysionetFileName));
    for (int i = 0; i < param->GetLeadCnt(); i++)
    {
        size_t cnt = 0;
        wcstombs_s(&cnt, paFN[i], MAX_PATH, s, _TRUNCATE);
        if (!cnt)
        {
            MessageBox::Show("Путь и имя файла не должны содержать русских букв и пробелов", "WFDB Library Error",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
            return false;
        }
        paSI[i].fname = paFN[i];

        while ((param->m_MaskLeads & (1i64 << curChanInd)) == 0)
            curChanInd++;
        CComBSTR cName;
        RawData->get_ChannelName(curChanInd, &cName);
        wcstombs_s(&cnt, paDesc[i], 10, cName, _TRUNCATE);
        //		wcstombs(paDesc[i], cName, 10);
        paSI[i].desc = paDesc[i];

        paSI[i].units = 0;

        float flLSB = GetLSB(RawData, curChanInd);
        if (param->m_ExportFiter & EFM_LSB)
        {
            IKTCompLinker * Linker(0);
            RawData->get_Linker(&Linker);
            IKTLSBFilter *f(0);
            Linker->ExistsComponentInterface(LSBFilter, (IKTCompDispatch**)&f);
            int mult;
            f->GetMultiplier(&mult);
            flLSB /= mult;
        }
        paSI[i].gain = (int)(1000 / flLSB);//  [2015/Февраль/5 yes]

        paSI[i].initval = 0;
        paSI[i].group = 0;
        paSI[i].fmt = 16;
        paSI[i].spf = 1;
        paSI[i].bsize = 0;
        paSI[i].adcres = 16;
        paSI[i].adczero = 0;
        paSI[i].baseline = 0;
        paSI[i].cksum = 0;
        paSI[i].nsamp = 0;
        curChanInd++;
    }

    return true;
}

void WriteData(IKTProgress *Progress, IKTRawData *RawData, SExportParameters *param)
{

    ULONG Frequency(257);
    RawData->get_Frequency(&Frequency);
    int BlockSize = Frequency * 600;
    LONG SP = param->m_StartPos;
    int nChan = param->GetLeadCnt();
    int nCurrPercent = -1;
    CComBSTR FilterName;
    FilterName.Attach(StringToBSTR(FilterMask2Str(param->m_ExportFiter)));
    while (param->m_EndPos > SP)
    {
        CComPtr<IKTECGBlock> Block;
        LONG PointCount = min(param->m_EndPos - SP, BlockSize);
        RawData->GetECGBlock(param->m_MaskLeads, SP, PointCount, FilterName, &Block);

        const int nMaxChan = sizeof(param->m_MaskLeads) * 8;
        CComPtr<IKTECGRawData> EcgData[nMaxChan];

        LONG* EcgPoints[nMaxChan];
        for (int i = 0; i < nChan; ++i)
        {
            Block->GetECGData((ULONG)i, &EcgData[i]);
            EcgData[i]->get_Points(SP, &EcgPoints[i]);
        }

        WFDB_Sample aData[nMaxChan];

        for (int j = 0; j < PointCount; j++)
        {
            for (int k = 0; k < param->GetLeadCnt(); k++)
            {
                LONG x = EcgPoints[k][j];
                if (x >= SHRT_MAX - 1)
                    aData[k] = SHRT_MAX - 1;
                else if (x <= SHRT_MIN + 1)
                    aData[k] = SHRT_MIN + 1;
                else
                    aData[k] = (short)x;
            }
            putvec(aData);

        }
        SP += PointCount;
        int nNewPercent = 50 + 50 * SP / (param->m_EndPos - param->m_StartPos);
        if (nCurrPercent != nNewPercent && Progress)
            Progress->SetPercent(nCurrPercent = nNewPercent);
    }
}

void WriteRecordInfo(IKTRawData *RawData)
{
    ULONG age;
    RawData->get_Age(&age);
    CComBSTR strsex;
    RawData->get_Sex(&strsex);
    CString ssex = String::Compare(gcnew String(strsex), L"ж", true) == 0 ? "F" : "M";

   
    CString info;
    info.Format("<age>: %d <sex>: %s", age, ssex);
    putinfo(CString::PCXSTR(info));
}

void SaveECGHeader(IKTRawData * RawData, SExportParameters * param, WFDB_Siginfo * aSIб, bool bHeaderOnly);

extern "C" __declspec(dllexport) EExportErr ExportECG2Physionet(IKTRawData *RawData, IKTProgress *Progress, SExportParameters *param, bool bHeaderOnly)
{
    if (param->m_StartPos == param->m_EndPos)
        return EEE_OK;

    if (Progress)
        Progress->SetPercent(0);

    const int nMaxChan = sizeof(param->m_MaskLeads) * 8;

    WFDB_Siginfo aSI[nMaxChan];

    char aFN[nMaxChan][MAX_PATH];
    char *(apFN)[nMaxChan];
    char aDesc[nMaxChan][11];
    char *(apDesc)[nMaxChan];
    for (int k = 0; k < nMaxChan; k++)
    {
        apFN[k] = &(aFN[k][0]);
        apDesc[k] = &(aDesc[k][0]);
    }
    if (!FillSiginfo(RawData, param, aSI, apFN, apDesc))
    {
        wfdbquit();//	throw ;
        return EEE_FileErr;
    }

    int nOpen = osigfopen(aSI, param->GetLeadCnt());
    if (!bHeaderOnly)
    {
        if (nOpen != param->GetLeadCnt())
        {
            wfdbquit();//	throw ;
            return EEE_FileErr;
        }
        WriteData(Progress, RawData, param);
    }

    wfdbquit();//	throw ;
    SaveECGHeader(RawData, param, aSI, bHeaderOnly);

    if (Progress)
        Progress->SetPercent(-1);

    return EEE_OK;
}

void SaveECGHeader(IKTRawData* RawData, SExportParameters* param, WFDB_Siginfo* aSI, bool bHeaderOnly)
{
  ULONG Frequency(257);
  RawData->get_Frequency(&Frequency);
  setsampfreq(Frequency);

  //	create .hea file
  String^ RecName = param->m_ECGPhysionetFileName->Replace(".dat", "");
  String^ path = IO::Path::GetDirectoryName(RecName);
  IO::Directory::SetCurrentDirectory(path);
  RecName = RecName->Substring(RecName->LastIndexOf("\\") + 1);
  char record[MAX_PATH];
  CComBSTR Name;
  Name.Attach(StringToBSTR(RecName));
  unsigned int ReturnValue;
  wcstombs_s(&ReturnValue, record, MAX_PATH, (wchar_t*)Name, Name.Length());
  newheader(record);
  //  [2015/Август/14 yes]
  //	remove path from signal info
  for (int i = 0; i < param->GetLeadCnt(); i++)
  {
    char* res = strrchr(aSI[i].fname, '\\');
    aSI[i].fname = res ? res + 1 : aSI[i].fname;
    aSI[i].nsamp = param->m_EndPos - param->m_StartPos;
  }
  setheader(record, aSI, param->GetLeadCnt());

  // WriteRecordInfo(RawData);

  wfdbquit();

  //Add time
  String^ sFirstLine = String::Empty;
  String^ sAllText = String::Empty;
  String^ sHeaderPath = IO::Path::ChangeExtension(param->m_ECGPhysionetFileName, L".hea");
  auto Stream = gcnew IO::FileStream(sHeaderPath, FileMode::Open, FileAccess::Read, FileShare::Write);
  try
  {
    IO::StreamReader^ sr = gcnew StreamReader(Stream);
    try
    {
      sFirstLine = sr->ReadLine();
      sAllText = sr->ReadToEnd();
    }
    finally
    {
      sr->Close();
    }
  }
  finally
  {
    Stream->Close();
  }
  if (!String::IsNullOrWhiteSpace(sFirstLine) && !String::IsNullOrWhiteSpace(sAllText))
  {
    Stream = gcnew IO::FileStream(sHeaderPath, FileMode::Create, FileAccess::Write, FileShare::Write);
    try
    {
      IO::StreamWriter^ sw = gcnew IO::StreamWriter(Stream);
      try
      {
        CComBSTR bstrName;          RawData->get_Name(&bstrName);
        CComBSTR bstrPatronymic;    RawData->get_Surname(&bstrPatronymic);
        CComBSTR bstrFamily;        RawData->get_Family(&bstrFamily);

        String^ sF = gcnew String(bstrFamily); sF = KtCommon::CTransliteration::Front(sF);
        String^ sN = gcnew String(bstrName); sN = KtCommon::CTransliteration::Front(sN);
        String^ sP = gcnew String(bstrPatronymic); sP = KtCommon::CTransliteration::Front(sP);
        String^ sName = String::Format(L"#<family>: {0} <name>: {1} <patronymic>: {2}", sF, sN, sP);

        DATE Day;
        RawData->get_Start((DATE*)&Day);
        DateTime datetime = DateTime::FromOADate(Day);
        String^ sTime = datetime.TimeOfDay.ToString(L"c");
        String^ sNewAllText = sFirstLine + L" " + sTime + System::Environment::NewLine + sAllText + "#" + Environment::NewLine + sName;
        sw->Write(sNewAllText);
      }
      finally
      {
        sw->Close();
      }
    }
    finally
    {
      Stream->Close();
    }
  }
}



