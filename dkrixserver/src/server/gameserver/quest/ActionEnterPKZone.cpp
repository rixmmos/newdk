////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionEnterPKZone.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionEnterPKZone.h"

#include <stdio.h>

#include "CastleInfoManager.h"
#include "GCModifyInformation.h"
#include "GCMoveOK.h"
#include "GCNPCResponse.h"
#include "GCSystemMessage.h"
#include "GCUpdateInfo.h"
#include "GQuestManager.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "Ousters.h"
#include "PCOustersInfo2.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "PKZoneInfoManager.h"
#include "PacketUtil.h"
#include "PaySystem.h"
#include "Properties.h"
#include "Slayer.h"
#include "StringPool.h"
#include "StringStream.h"
#include "SystemAvailabilitiesManager.h"
#include "Vampire.h"
#include "WeatherManager.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "ZoneUtil.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionEnterPKZone::read(PropertyBuffer& pb)

{
    __BEGIN_TRY

    try {
        m_ZoneID = pb.getPropertyInt("ZoneID");
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionEnterPKZone::execute(Creature* pNPC, Creature* pCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pCreature != NULL);
    Assert(pCreature->isPC());

    SYSTEM_RETURN_IF_NOT(SYSTEM_PK_ZONE);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

    Assert(pPC != NULL);

    bool bTransport = true;
#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
    try {
        ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_ZoneID);

        
        if (pZoneInfo == NULL || pZoneInfo->isPayPlay() && !pGamePlayer->isPayPlaying()) {
            string connectIP = pGamePlayer->getSocket()->getHost();

            
            if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID())) {
                sendPayInfo(pGamePlayer);
            } else {
                
                GCSystemMessage gcSystemMessage;

                if (g_pConfig->getPropertyInt("IsNetMarble") == 0) {
                    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER_PAY_ZONE));
                } else {
                    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER));
                }

                pGamePlayer->sendPacket(&gcSystemMessage);

                bTransport = false;

                return;
            }
        }
    } catch (NoSuchElementException&) {
    }
#endif

    if (bTransport) {
        if (pPC->isPLAYER() && !g_pPKZoneInfoManager->canEnterPKZone(m_ZoneID))
            bTransport = false;
    }

    if (bTransport) {
        PKZoneInfo* pPKZoneInfo = g_pPKZoneInfoManager->getPKZoneInfo(m_ZoneID);

        if (pPKZoneInfo == NULL) {
            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CANNOT_ENTER));
            pGamePlayer->sendPacket(&gcSystemMessage);
        }

        
        if (!pPC->isPLAYER() || pPKZoneInfo->enterZone()) {
            if (pPC->isSlayer()) {
                Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
                Assert(pSlayer != NULL);

                
                if (pSlayer->hasRideMotorcycle()) {
                    pSlayer->getOffMotorcycle();
                }
            }

            if (pPC->isOusters()) {
                Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
                Assert(pOusters != NULL);

                
                if (pOusters->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)) {
                    Effect* pEffect = pOusters->findEffect(Effect::EFFECT_CLASS_SUMMON_SYLPH);
                    if (pEffect != NULL)
                        pEffect->setDeadline(0);
                }
            }

            if (pNPC != NULL)
                pPC->getGQuestManager()->illegalWarp();
            transportCreature(pCreature, m_ZoneID, pPKZoneInfo->getEnterX(), pPKZoneInfo->getEnterY(), true);
        } else {
            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_PKZONE_PC_LIMITED));
            pGamePlayer->sendPacket(&gcSystemMessage);
        }
    } else {
        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_PKZONE_PC_LIMITED));
        pGamePlayer->sendPacket(&gcSystemMessage);
    }

    __END_DEBUG
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionEnterPKZone::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionEnterPKZone("
        << "ZoneID:" << (int)m_ZoneID << ")";
    return msg.toString();

    __END_CATCH
}
