//////////////////////////////////////////////////////////////////////////////
// Filename    : EventResurrect.cpp
// Written by  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventResurrect.h"

#include "GCUpdateInfo.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "Ousters.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "PacketUtil.h"
#include "PlayerStatus.h"
#include "Slayer.h"
#include "TimeManager.h"
#include "Vampire.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"

//////////////////////////////////////////////////////////////////////////////
// class EventResurrect member methods
//////////////////////////////////////////////////////////////////////////////

EventResurrect::EventResurrect(GamePlayer* pGamePlayer)

    : Event(pGamePlayer) {
    //	m_pResurrectZone = NULL;
    //	m_X = m_Y = 0;
}

EventResurrect::~EventResurrect()

{}

void EventResurrect::activate()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(m_pGamePlayer != NULL);

    Creature* pDeadPC = m_pGamePlayer->getCreature();

    Assert(pDeadPC != NULL);

    
    pDeadPC->removeFlag(Effect::EFFECT_CLASS_HIDE);

    
    if (pDeadPC->isVampire() && pDeadPC->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)) {
        pDeadPC->setMoveMode(Creature::MOVE_MODE_FLYING);
    } else {
        pDeadPC->setMoveMode(Creature::MOVE_MODE_WALKING);
    }

    
    if (pDeadPC->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pDeadPC);
        pSlayer->setHP(pSlayer->getHP(ATTR_MAX), ATTR_CURRENT);
    } else if (pDeadPC->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pDeadPC);
        pVampire->setHP(pVampire->getHP(ATTR_MAX), ATTR_CURRENT);
    } else if (pDeadPC->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pDeadPC);
        pOusters->setHP(pOusters->getHP(ATTR_MAX), ATTR_CURRENT);
    }

    
    Zone* pOldZone = pDeadPC->getZone();
    Assert(pOldZone != NULL);

    try {
        
        pOldZone->getZoneGroup()->getZonePlayerManager()->deletePlayer(m_pGamePlayer->getSocket()->getSOCKET());

        
        m_pGamePlayer->setPlayerStatus(GPS_WAITING_FOR_CG_READY);

        
        // g_pIncomingPlayerManager->pushPlayer(m_pGamePlayer);
        pOldZone->getZoneGroup()->getZonePlayerManager()->pushOutPlayer(m_pGamePlayer);

    } catch (NoSuchElementException& t) {
        filelog("eventRessurect.txt", "%s-%s", t.toString().c_str(), pDeadPC->getName().c_str());
        cerr << "EventResurrect::activate() : NoSuchElementException" << endl;
        
        
        
        //  by sigi. 2002.11.25
    }

    

    
    
    // pDeadPC->registerObject();

     

    __END_DEBUG
    __END_CATCH
}

string EventResurrect::toString() const

{
    StringStream msg;
    msg << "EventResurrect("
        << ")";
    return msg.toString();
}
