//////////////////////////////////////////////////////////////////////////////
// Filename    : EventAuth.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventAuth.h"

#include "Creature.h"
#include "EventKick.h"
#include "GCAuthKey.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "Item.h"
#include "Ousters.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
EventAuth::EventAuth(GamePlayer* pGamePlayer)

    : Event(pGamePlayer) {
    
    setDeadline(600);
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
EventAuth::~EventAuth()

{}

//////////////////////////////////////////////////////////////////////////////
// activate
//////////////////////////////////////////////////////////////////////////////
void EventAuth::activate()

{
    __BEGIN_TRY

    if (m_pGamePlayer->getPlayerStatus() == GPS_NORMAL) {
        Assert(m_pGamePlayer != NULL);

        // if ( !m_pGamePlayer->getCSAuth().IsAuth() )
        if (true) {
            filelog("CSAuth.log", "[%s]    .", m_pGamePlayer->getID().c_str());

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage("nProtect GameGuard .GameGuard.");
            m_pGamePlayer->sendPacket(&gcSystemMessage);

            EventKick* pKick = new EventKick(m_pGamePlayer);
            pKick->setDeadline(100);
            
            pKick->sendMessage();

            m_pGamePlayer->addEvent(pKick);
        } else {
            // DWORD key = m_pGamePlayer->getCSAuth().GetAuthDword();
            // GCAuthKey gcKey;
            // gcKey.setKey(key);
            // m_pGamePlayer->sendPacket(&gcKey);
        }
    }

    
    Timeval delay;
    delay.tv_sec = 300;
    delay.tv_usec = 0;

    m_Deadline = m_Deadline + delay;

    __END_CATCH
}
