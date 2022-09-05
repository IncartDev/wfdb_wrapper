#include "StdAfx.h"
#include "createPhysionetHeader.h"

extern "C" __declspec( dllexport ) EExportErr ExportECG2Physionet(IKTRawData *RawData, IKTProgress *Progress, SExportParameters *param, bool bHeaderOnly);

using namespace System;

void CCreatePhysionetHeader::Run()
{
  IKTRawData *RawData = GetObject<IKTRawData>(L"RawData");
  
  SExportParameters param(RawData);
  CComBSTR FileName;
  RawData->get_FileName(VARIANT_FALSE,&FileName);
  param.m_ECGPhysionetFileName = gcnew String(FileName);

  ExportECG2Physionet(RawData, 0, &param, true);
}
