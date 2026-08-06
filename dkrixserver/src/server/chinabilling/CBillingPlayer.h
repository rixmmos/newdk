/////////////////////////////////////////////////////////////////////
//
// filename		: CBillingPlayer.h
//
/////////////////////////////////////////////////////////////////////

#ifndef __CBILLING_PLAYER__
#define __CBILLING_PLAYER__

// include file
#include "Exception.h"
#include "Player.h"
#include "Types.h"

// forward declaration
class Packet;
class CBillingPacketHeader;
class CBillingPacketErrorBody;

#ifdef __LOGIN_SERVER__
class LoginPlayer;
class CBillingPacketResponseLoginBody;
#elif defined(__GAME_SERVER__)
class GamePlayer;
class CBillingPacketResponseIntervalValidationBody;
class CBillingPacketResponseLoginBody;
class CBillingPacketResponseMinusPointBody;
class CBillingPacketResponseMinusMinuteBody;
class CBillingPacketResponseLogoutBody;
#endif

/////////////////////////////////////////////
// class CBillingPlayer
/////////////////////////////////////////////

class CBillingPlayer : public Player {
public:
    // constructor & destructor
    CBillingPlayer();
    CBillingPlayer(Socket* pSocket);

    virtual ~CBillingPlayer();

public:
    // network read / write
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
#ifdef __LOGIN_SERVER__
    void sendLogin(LoginPlayer* pLoginPlayer);
#elif defined(__GAME_SERVER__)
    void sendIntervalValidation();
    void sendLogin(GamePlayer* pGamePlayer);
    void sendMinusPoint(GamePlayer* pGamePlayer);
    void sendMinusMinute(GamePlayer* pGamePlayer);
    void sendLogout(GamePlayer* pGamePlayer);
#endif

public:
    
    void executeError(CBillingPacketHeader& header, CBillingPacketErrorBody& body);
#ifdef __LOGIN_SERVER__
    void executeLogin(CBillingPacketHeader& header, CBillingPacketResponseLoginBody& body);
#elif defined(__GAME_SERVER__)
    void executeIntervalValidation(CBillingPacketHeader& header, CBillingPacketResponseIntervalValidationBody& body);
    void executeLogin(CBillingPacketHeader& header, CBillingPacketResponseLoginBody& body);
    void executeMinusPoint(CBillingPacketHeader& header, CBillingPacketResponseMinusPointBody& body);
    void executeMinusMinute(CBillingPacketHeader& header, CBillingPacketResponseMinusMinuteBody& body);
    void executeLogout(CBillingPacketHeader& header, CBillingPacketResponseLogoutBody& body);
#endif

#ifdef __GAME_SERVER__
    
    void logPacket(CBillingPacketHeader* header, CBillingPacketResponseLoginBody* login,
                   CBillingPacketResponseMinusPointBody* point, CBillingPacketResponseLogoutBody* logout,
                   CBillingPacketErrorBody* error);
#endif

public:
#ifdef __GAME_SERVER__
    void checkSessionID(CBillingPacketHeader& header, GamePlayer* pGamePlayer);
#endif
};

#endif
