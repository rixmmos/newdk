#include "EffectKickOut.h"

#include <cstdio>

#include "GCSystemMessage.h"
#include "PCManager.h"
#include "Zone.h"

void EffectKickOut::affect() {
    
    setNextTime(600);
    m_MinutesCount--;

    char msg[200];
    sprintf(msg, "%d.", m_MinutesCount);
    GCSystemMessage gcSM;
    gcSM.setMessage(msg);
    m_pZone->broadcastPacket(&gcSM);
}

void EffectKickOut::unaffect() {
    __BEGIN_TRY

    GCSystemMessage gcSM;
    gcSM.setMessage(". 10.");
    m_pZone->broadcastPacket(&gcSM);

    if (m_pZone != NULL)
        m_pZone->getPCManager()->transportAllCreatures(0xffff);

    __END_CATCH
}
