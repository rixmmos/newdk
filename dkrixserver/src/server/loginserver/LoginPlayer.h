//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginPlayer.h
// Written by  : reiot@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __LOGIN_PLAYER_H__
#define __LOGIN_PLAYER_H__

// include files
#include <deque>

#include "GCReconnectLogin.h"
#include "Mutex.h"
#include "Packet.h"
#include "PaySystem.h"
#include "Player.h"
#include "PlayerStatus.h"
#include "Timeval.h"
#include "chinabilling/CBillingPlayerInfo.h"
#include "gameserver/billing/BillingPlayerInfo.h"

class LCPCList;

//////////////////////////////////////////////////////////////////////
//
// class LoginPlayer
//

//



//




//
//////////////////////////////////////////////////////////////////////

class LoginPlayer : public Player, public PaySystem, public BillingPlayerInfo, public CBillingPlayerInfo {
public:
    
    static const uint nPacketHistory = 10;

    static const uint maxFailure = 3;

public:
    // constructor
    LoginPlayer(Socket* pSocket);

    // destructor
    ~LoginPlayer();

    // read socket's receive buffer and fill input buffer
    // virtual void processInput () throw ( IOException , Error );

    // parse packet and execute handler for the packet
    virtual void processCommand(bool Option = true);

    // flush output buffer to socket's send buffer
    // virtual void processOutput () throw ( IOException , Error );

    // send packet to player's output buffer
    virtual void sendPacket(Packet* packet);

    // disconnect
    
    virtual void disconnect(bool bDisconnected = DISCONNECTED);
    virtual void disconnect_nolog(bool bDisconnected = DISCONNECTED);


    // get debug string
    virtual string toString() const;

public:
    // return recent N-th packet
    
    Packet* getOldPacket(uint prev = 0);

    // return recent packet which has packetID
    
    Packet* getOldPacket(PacketID_t packetID);

    // get/set player's status
    PlayerStatus getPlayerStatus() const throw() {
        return m_PlayerStatus;
    }
    void setPlayerStatus(PlayerStatus playerStatus) throw() {
        m_PlayerStatus = playerStatus;
    }

    
    uint getFailureCount() const throw() {
        return m_FailureCount;
    }
    void setFailureCount(uint nFailed) throw() {
        m_FailureCount = nFailed;
    }

    // get / set GoreLevel
    bool isAdult() const throw() {
        return m_isAdult;
    }
    void setAdult(bool isAdult) throw() {
        m_isAdult = isAdult;
    }

public:
    int getKickCharacterCount() const throw() {
        return m_KickCharacterCount;
    }
    void setExpireTimeForKickCharacter();

    
    WorldID_t getWorldID() const throw() {
        return m_WorldID;
    }
    void setWorldID(WorldID_t WorldID) throw() {
        m_WorldID = WorldID;
    }

    
    WorldID_t getGroupID() const throw() {
        return m_ServerGroupID;
    }
    void setGroupID(ServerGroupID_t ServerGroupID) throw() {
        m_ServerGroupID = ServerGroupID;
    }

    
    uint getLastSlot() const throw() {
        return m_LastSlot;
    }
    void setLastSlot(uint lastSlot) throw() {
        m_LastSlot = lastSlot;
    }

    
    bool isSetWorldGroupID() const throw() {
        return m_bSetWorldGroupID;
    }
    void setWorldGroupID(bool bSet) throw() {
        m_bSetWorldGroupID = bSet;
    }

    
    const string& getLastCharacterName() const throw() {
        return m_LastCharacterName;
    }
    void setLastCharacterName(const string& name) throw() {
        m_LastCharacterName = name;
    }

    const string& getZipcode() const throw() {
        return m_Zipcode;
    }
    void setZipcode(const string& zipcode) throw() {
        m_Zipcode = zipcode;
    }

    const string& getSSN() const throw() {
        return m_SSN;
    }
    void setSSN(const string& ssn) {
        m_SSN = ssn;
    }

    bool isFreePass() const {
        return m_bFreePass;
    }
    void setFreePass(bool bFreePass = true) {
        m_bFreePass = bFreePass;
    }

    bool isWebLogin() const {
        return m_bWebLogin;
    }
    void setWebLogin(bool bWebLogin = true) {
        m_bWebLogin = bWebLogin;
    }

    void makePCList(LCPCList& lcPCList);

public:
    void sendLGKickCharacter();
    void sendLCLoginOK();

public:
    void setBillingSession() {
        BillingPlayerInfo::setBillingSession(this);
    }
    bool sendBillingLogin();

public:
    void setGameServerIP(const string& ip) {
        m_gameServerIP = ip;
    }
    const string& getGameServerIP() {
        return m_gameServerIP;
    }

private:
    // previous packet queue
    deque<Packet*> m_PacketHistory;

    // player status
    PlayerStatus m_PlayerStatus;

    // expire time
    Timeval m_ExpireTime;

    
    uint m_FailureCount;

    // mutex
    mutable Mutex m_Mutex;

    
    bool m_bSetWorldGroupID;
    WorldID_t m_WorldID;
    ServerGroupID_t m_ServerGroupID;
    uint m_LastSlot;
    string m_LastCharacterName;

    
    string m_SSN;
    string m_Zipcode;

    bool m_isAdult;

    
    uint m_KickCharacterCount;
    Timeval m_ExpireTimeForKickCharacter;

    
    bool m_bFreePass;

    
    bool m_bWebLogin;

    // m_gameServerIP is set in CLSelectPCHandler.
    string m_gameServerIP;
};

#endif
