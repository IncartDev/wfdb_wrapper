
#include "stdafx.h"
#include "PhysionetAnnotationFile.h"  

#include "../Common/Interfaces/IKTLinker.h"  
#include "createPhysionetHeader.h"

extern "C" __declspec(dllexport) KTDeletable *Create(const wchar_t *objectName, IKTLinker *Linker, const wchar_t *instanceName)
{
  if (wcscmp(objectName, L"PhysionetAnnotationFile") == 0)
    return new CPhysionetAnnotationFile;
  else if (wcscmp(objectName, L"CreatePhysionetHeader") == 0)
    return new CCreatePhysionetHeader(Linker, instanceName);

  return 0;
}
