#pragma once

#include "../Common/ComInterfaces/IKTRawData.h"
#include "../Common/ComInterfaces/IntIds.h"
#include "../Common/Export/Export.h"

#include "../Common/Infrastructure/KTRichObject.h"

#include "../Common/Interfaces/IKTAlgorithm.h"

class CCreatePhysionetHeader : public KTRichObject, public IKTAlgorithm
{
public:
  CCreatePhysionetHeader(IKTLinker *L, const wchar_t *instanceName) : KTRichObject(L, instanceName) {}

  virtual void Run();
};
