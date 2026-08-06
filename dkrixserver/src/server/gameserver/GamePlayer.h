//////////////////////////////////////////////////////////////////////////////
// Filename    : GamePlayer.h
// Written by  : reiot@ewestsoft.com

//////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

#include <bitset>
#include <deque>

#include "EventManager.h"
#include "Exception.h"
#include "GCReconnectLogin.h"
#include "Mutex.h"
#include "Packet.h"
#include "PaySystem.h"
#include "Player.h"
#include "PlayerStatus.h"
#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"
#include "Timeval.h"
#include "Types.h"
#include "billing/BillingPlayerInfo.h"
#include "chinabilling/CBillingPlayerInfo.h"
#include "skill/Skill.h"

// #include "gameguard/CSAuth.h"

//////////////////////////////////////////////////////////////////////////////
// class GamePlayer
//

//



//




//////////////////////////////////////////////////////////////////////////////

class Creature;

class GamePlayer : public Player, public PaySystem, public BillingPlayerInfo, public CBillingPlayerInfo {
public:
    
    const static BYTE nPacketHistorySize = 10;

public:
    GamePlayer(Socket* pSocket);
    ~GamePlayer();

public:
    // read socket's receive buffer and fill input buffer
    // virtual void processInput() ;

    // parse packet and execute handler for the packet
    virtual void processCommand(bool Option = true);

    // flush output buffer to socket's send buffer
    virtual void processOutput();

    // send packet to player's output buffer
    virtual void sendPacket(Packet* packet);

    // disconnect
    
    virtual void disconnect(bool bDisconnected = DISCONNECTED);

    // get debug string
    virtual string toString() const;

    
    virtual bool verifySpeed(Packet* pPacket);

    // get creature pointer
    Creature* getCreature() {
        return m_pCreature;
    }
    const Creature* getCreature() const {
        return m_pCreature;
    }

    // set creature pointer
    void setCreature(Creature* pCreature) {
        m_pCreature = pCreature;
    }

    // return recent N-th packet
    
    Packet* getOldPacket(uint prev = 0);

    // return recent packet which has packetID
    
    Packet* getOldPacket(PacketID_t packetID);

    // get player's status
    PlayerStatus getPlayerStatus() const {
        return m_PlayerStatus;
    }

    // set player's status
    void setPlayerStatus(PlayerStatus playerStatus) {
        m_PlayerStatus = playerStatus;
    }

    //
    void addEvent(Event* pEvent);
    Event* getEvent(Event::EventClass EClass);
    void deleteEvent(Event::EventClass EClass);

    
    // Set Flag
    void setPenaltyFlag(PenaltyType PenaltyFlag) {
        m_PenaltyFlag.set(PenaltyFlag);
    }

    // remove Flag
    void removePenaltyFlag(PenaltyType PenaltyFlag) {
        m_PenaltyFlag.reset(PenaltyFlag);
    }

    // Is Flag?
    bool isPenaltyFlag(PenaltyType PenaltyFlag) {
        return m_PenaltyFlag.test(PenaltyFlag);
    }

public:
    uint getSpecialEventCount(void) const {
        return m_SpecialEventCount;
    }
    void setSpecialEventCount(uint count) {
        m_SpecialEventCount = count;
    }
    void loadSpecialEventCount(void);
    void saveSpecialEventCount(void);

public: 
    bool isKickForLogin() const {
        return m_bKickForLogin;
    }
    void setKickForLogin(bool bKickForLogin = true) {
        m_bKickForLogin = bKickForLogin;
    }

    const string& getKickRequestHost() const {
        return m_KickRequestHost;
    }
    uint getKickRequestPort() const {
        return m_KickRequestPort;
    }

    void setKickRequestHost(const string& host) {
        m_KickRequestHost = host;
    }
    void setKickRequestPort(uint port) {
        m_KickRequestPort = port;
    }

public:
    
    void setReconnectPacket(GCReconnectLogin* pPacket) {
        SAFE_DELETE(m_pReconnectPacket);
        m_pReconnectPacket = pPacket;
    }
    GCReconnectLogin* getReconnectPacket() const {
        return m_pReconnectPacket;
    }

    // by sigi. 2002.10.23
    bool isFreePass() const {
        return m_bFreePass;
    }
    void setFreePass(bool bFreePass = true) {
        m_bFreePass = bFreePass;
    }

public:
    void lock() {
        m_Mutex.lock();
    }
    void unlock() {
        m_Mutex.unlock();
    }

public:
    void setBillingSession() {
        BillingPlayerInfo::setBillingSession(this);
    }
    bool sendBillingLogin();

    void sendCBillingPayInfo();

    
    // by sigi. 2002.11.27
    void setEncryptCode();

public:
    void kickPlayer(uint nSeconds, uint KickMessageType);

    //////////////////////////////////////////////////
    
    //////////////////////////////////////////////////
public:
    bool loginPayPlay(PayType payType, const string& PayPlayDate, int PayPlayHours, uint payPlayFlag, const string& ip,
                      const string& playerID);

    bool loginPayPlay(const string& ip, const string& playerID);

    bool updatePayPlayTime(const string& playerID, const VSDateTime& currentDateTime, const Timeval& currentTime);

    void logoutPayPlay(const string& playerID, bool bClear = false, bool bDecreaseTime = true);

    bool isPayPlaying() const;

    bool isMetroFreePlayer() const {
        return m_bMetroFreePlayer;
    }
    void setMetroFreePlayer(bool bMetroFreePlayer = true) {
        m_bMetroFreePlayer = bMetroFreePlayer;
    }

    int getItemRatioBonusPoint(void) const {
        return m_ItemRatioBonusPoint;
    }
    void setItemRatioBonusPoint(int point) {
        m_ItemRatioBonusPoint = point;
    }

    bool startPacketLog(uint sec);
    // CCSAuth&	getCSAuth() { return m_NProtectCSAuth; }

    void logLoginoutDateTime();

#ifdef __THAILAND_SERVER__
    bool getPermission(void) const {
        return m_bPermission;
    }
    void setPermission(bool permission) {
        m_bPermission = permission;
    }
#endif


private:
    void setPCRoomLottoStartTime();
    void checkPCRoomLotto(const Timeval& currentTime);
    void savePCRoomLottoTime();
    void giveLotto();
    // add by Coffee 2007-6-25
    void tv_sub(struct timeval* out, struct timeval* in);

private:
    // creature
    Creature* m_pCreature;

    // previous packet queue
    deque<Packet*> m_PacketHistory;

    // player status
    PlayerStatus m_PlayerStatus;

    // Panelty status
    bitset<PENALTY_TYPE_MAX> m_PenaltyFlag;

    // expire time
    Timeval m_ExpireTime;

    BYTE m_VerifyCount;

    
    Timeval m_SpeedVerify;
    Timeval m_MoveSpeedVerify;
    Timeval m_AttackSpeedVerify;
    Timeval m_SkillSpeedVerify[SKILL_MAX];

    // mutex
    mutable Mutex m_Mutex;

    EventManager m_EventManager;

    
    
    uint m_SpecialEventCount;

    
    bool m_bKickForLogin;
    string m_KickRequestHost;
    uint m_KickRequestPort;

    
    GCReconnectLogin* m_pReconnectPacket;

    bool m_bFreePass;

    
    bool m_bMetroFreePlayer;

    
    int m_ItemRatioBonusPoint;

    Timeval m_PCRoomLottoStartTime; 
    uint m_PCRoomLottoSumTime;      

    string m_PacketLogFileName;
    bool m_bPacketLog;
    Timeval m_PacketLogEndTime;

    // CCSAuth			m_NProtectCSAuth;

    VSDateTime m_LoginDateTime;


#ifdef __THAILAND_SERVER__
    bool m_bPermission;
#endif
    
private:
    BYTE m_Sequence;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class isSamePlayer {
public:
    isSamePlayer(GamePlayer* pGamePlayer) : m_pGamePlayer(pGamePlayer) {}
    bool operator()(GamePlayer* pGamePlayer) {
        return pGamePlayer->getID() == m_pGamePlayer->getID();
    }

private:
    // Creature Pointer
    GamePlayer* m_pGamePlayer;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class isSamePlayerbyID {
public:
    isSamePlayerbyID(const string& ID) : m_ID(ID) {}
    bool operator()(GamePlayer* pGamePlayer) {
        return pGamePlayer->getID() == m_ID;
    }

private:
    // Creature ID
    string m_ID;
};

#endif
