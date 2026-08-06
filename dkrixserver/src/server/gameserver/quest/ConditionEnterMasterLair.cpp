////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionEnterMasterLair.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ConditionEnterMasterLair.h"

#include "DB.h"
#include "FlagSet.h"
#include "GamePlayer.h"
#include "MasterLairManager.h"
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
bool ConditionEnterMasterLair::isSatisfied(Creature* pCreature1, Creature* pCreature2, void* pParam) const

{
    if (!g_pVariableManager->isActiveMasterLair()) {
        return false;
    }

    Assert(pCreature2 != NULL);
    Assert(pCreature2->isPC());

    // cout << "Condition: EnterMasterLair" << endl;

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

        
        if (!pZone->isMasterLair()) {
            return true;
        }

        MasterLairManager* pMasterLairManager = pZone->getMasterLairManager();
        Assert(pMasterLairManager != NULL);

        if (pMasterLairManager->enterCreature(pCreature2)) {
            
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ConditionEnterMasterLair::read(PropertyBuffer& propertyBuffer)

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
string ConditionEnterMasterLair::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ConditionEnterMasterLair("
        << "TargetZoneID:" << (int)m_TargetZoneID << ")";
    return msg.toString();

    __END_CATCH
}
