////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionEnterCastle.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ConditionEnterCastle.h"

#include "CastleInfoManager.h"
#include "DB.h"
#include "FlagSet.h"
#include "GamePlayer.h"
#include "PacketUtil.h"
#include "PaySystem.h"
#include "PlayerCreature.h"
#include "VariableManager.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"

////////////////////////////////////////////////////////////////////////////////
// is satisfied?
////////////////////////////////////////////////////////////////////////////////
bool ConditionEnterCastle::isSatisfied(Creature* pCreature1, Creature* pCreature2, void* pParam) const

{
    /*if (!g_pVariableManager->isActiveCastle())
    {
        return false;
    }*/
    

    Assert(pCreature2 != NULL);
    Assert(pCreature2->isPC());

    // cout << "Condition: EnterCastle" << endl;

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
        
        Zone* pZone = getZoneByZoneID(m_TargetZoneID);
        Assert(pZone != NULL);

        
        if (!pZone->isCastle()) {
            return true;
        }

        
        
        
        
        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);

        return g_pCastleInfoManager->canPortalActivate(m_TargetZoneID, pPC);
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ConditionEnterCastle::read(PropertyBuffer& propertyBuffer)

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
string ConditionEnterCastle::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ConditionEnterCastle("
        << "TargetZoneID:" << (int)m_TargetZoneID << ")";
    return msg.toString();

    __END_CATCH
}
