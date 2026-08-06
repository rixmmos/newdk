//////////////////////////////////////////////////////////////////////////////
// Filename    : EventCBilling.cpp
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EventCBilling.h"

#include "Assert.h"
#include "GamePlayer.h"
#include "chinabilling/CBillingPlayerManager.h"


//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
EventCBilling::EventCBilling(GamePlayer* pGamePlayer)

    : Event(pGamePlayer), m_TrySendLoginCount(0) {}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
EventCBilling::~EventCBilling()

{}


//////////////////////////////////////////////////////////////////////////////
// activate
//////////////////////////////////////////////////////////////////////////////
void EventCBilling::activate()

{
    __BEGIN_TRY

    Assert(m_pGamePlayer != NULL);

    if (!m_pGamePlayer->isCBillingVerified()) {
        
        m_pGamePlayer->setCBillingVerified(g_pCBillingPlayerManager->sendLogin(m_pGamePlayer));
        m_TrySendLoginCount++;
    }

    
    if (m_pGamePlayer->isCBillingVerified()) {
        
        if (!g_pCBillingPlayerManager->sendMinusPoint(m_pGamePlayer)) {
            m_pGamePlayer->increaseMissedMinusPointPacket();
        }

        // send missed minus point packet
        while (m_pGamePlayer->hasMissedMinusPointPacket()) {
            if (g_pCBillingPlayerManager->sendMinusPoint(m_pGamePlayer)) {
                m_pGamePlayer->decreaseMissedMinusPointPacket();
            } else {
                break;
            }
        }
    } else {
        
        m_pGamePlayer->increaseMissedMinusPointPacket();
    }

    
    static int interval = g_pCBillingPlayerManager->getMinusIntervalInt();

    Timeval delay;
    delay.tv_sec = interval * 60;
    delay.tv_usec = 0;

    m_Deadline = m_Deadline + delay;

    __END_CATCH
}
