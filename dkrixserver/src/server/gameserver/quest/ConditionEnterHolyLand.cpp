////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionEnterHolyLand.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ConditionEnterHolyLand.h"

#include "DB.h"
#include "FlagSet.h"
#include "GamePlayer.h"
#include "PacketUtil.h"
#include "PaySystem.h"
#include "PlayerCreature.h"
#include "VariableManager.h"
#include "WarSystem.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"

////////////////////////////////////////////////////////////////////////////////
// is satisfied?
////////////////////////////////////////////////////////////////////////////////
bool ConditionEnterHolyLand::isSatisfied(Creature* pCreature1, Creature* pCreature2, void* pParam) const

{
    /*if (!g_pVariableManager->isActiveHolyLand())
    {
        return false;
    }*/
    

    Assert(pCreature2 != NULL);
    Assert(pCreature2->isPC());

    // cout << "Condition: EnterHolyLand" << endl;

    bool bPayPlay = false;

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature2->getPlayer());
    Assert(pGamePlayer != NULL);

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
    
    
    if (pGamePlayer->isPayPlaying() || pGamePlayer->isFamilyFreePass()) {
        bPayPlay = true;
    } else {
        
        string connectIP = pGamePlayer->getSocket()->getHost();

        if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID())) {
            sendPayInfo(pGamePlayer);
            bPayPlay = true;
        }
    }
#else
    bPayPlay = true;
#endif

    
    if (bPayPlay) {
        
        if (g_pWarSystem->hasActiveRaceWar() && g_pVariableManager->isActiveRaceWarLimiter()) {
            Zone* pZone = getZoneByZoneID(m_TargetZoneID);
            Assert(pZone != NULL);

            
            if (!pZone->isHolyLand()) {
                return true;
            }

            PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);

            
            return pPC->isFlag(Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET);
        }

        
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ConditionEnterHolyLand::read(PropertyBuffer& propertyBuffer)

{
    try {
        // read turn
        m_TargetZoneID = propertyBuffer.getPropertyInt("TargetZoneID");
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ConditionEnterHolyLand::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ConditionEnterHolyLand("
        << "TargetZoneID:" << (int)m_TargetZoneID << ")";
    return msg.toString();

    __END_CATCH
}
