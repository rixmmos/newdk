#include "EffectGDRLairClose.h"

#include <cstdio>

#include "GCSystemMessage.h"
#include "GDRLairManager.h"
#include "ZoneGroupManager.h"

void EffectGDRLairClose::affect() {
    setNextTime(600);

    char msg[200];
    sprintf(msg, "%d,.", m_MinutesCount);
    GCSystemMessage gcSM;
    gcSM.setMessage(msg);
    g_pZoneGroupManager->broadcast(&gcSM);
    m_MinutesCount--;
}

void EffectGDRLairClose::unaffect() {
    __BEGIN_TRY

    cout << "  ." << endl;
    GDRLairManager::Instance().close();

    GCSystemMessage gcSM;
    gcSM.setMessage(".");
    g_pZoneGroupManager->broadcast(&gcSM);

    __END_CATCH
}
