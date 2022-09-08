#pragma once

#include "../Common/Interfaces/IPhysionetAnnotationFile.h"
#include "../Common/Infrastructure/Blocks/KTDeletable.h"


class CPhysionetAnnotationFile: public IPhysionetAnnotationFile, public KTDeletable
{
public:
    virtual ~CPhysionetAnnotationFile();
    virtual void Open(const wchar_t *FileName, bool bForWriting);
    virtual BOOL Write(long Point, unsigned char Type, signed char SubType, signed char Channel, const char *AddInfo, signed char Num);
  
  virtual bool Read(long *Point,unsigned char *Type, signed char *SubType, signed char *Channel, string *AddInfo);
  virtual void Rewind();
    virtual void Close();
    virtual const wchar_t * GetFileName();
private:
    wchar_t * m_FileName;

};
