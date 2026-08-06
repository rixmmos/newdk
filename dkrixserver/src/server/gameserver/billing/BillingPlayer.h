//////////////////////////////////////////////////////////////////////
//
// Filename    : BillingPlayer.h
// Written by  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __BILLING_PLAYER_H__
#define __BILLING_PLAYER_H__

// include files
#include "BillingInfo.h"
#include "Exception.h"
#include "Player.h"
#include "Types.h"

class CommonBillingPacket;

//////////////////////////////////////////////////////////////////////
//
// class BillingPlayer
//
//////////////////////////////////////////////////////////////////////

class BillingPlayer : public Player {
public:
    // constructor
    BillingPlayer();
    BillingPlayer(Socket* pSocket);

    // destructor
    virtual ~BillingPlayer();

    virtual void processInput();

    virtual void processOutput();

    // parse packet and execute handler for the packet
    virtual void processCommand();

    // send packet to player's output buffer
    virtual void sendPacket(Packet* pPacket);

    // disconnect
    
    
    
    virtual void disconnect(bool bDisconnected = DISCONNECTED);

    void setSocket(Socket* pSocket);

    // get debug string
    virtual string toString() const;

public:
    void sendPayInit();
    void sendPayCheck(CommonBillingPacket* pPacket);
    void sendPayLogin(Player* pPlayer);
    void sendPayLogout(Player* pPlayer);

    int getRetryCount() const {
        return m_RetryCount;
    }
    int getRetryValue() const {
        return m_RetryValue;
    }

    void resetRetryCount() {
        m_RetryCount = 0;
    }
    void resetRetryValue() {
        m_RetryValue = 0;
    }

protected:
    
    int m_RetryCount;
    
    int m_RetryValue;
};

#endif
