/////////////////////////////////////////////////////////////////////
//
// filename		: SBillingPlayer.h
//
/////////////////////////////////////////////////////////////////////

#ifndef __SBILLING_PLAYER__
#define __SBILLING_PLAYER__

// include file
#include "Exception.h"
#include "Player.h"
#include "Types.h"

// forward declaration
class PayUser;
class Packet;
class CBillingPacketHeader;
class CBillingPacketErrorBody;

class CBillingPacketResponseIntervalValidationBody;
class CBillingPacketResponseLoginBody;
class CBillingPacketResponseMinusPointBody;
class CBillingPacketResponseMinusMinuteBody;
class CBillingPacketResponseLogoutBody;

/////////////////////////////////////////////
// class SBillingPlayer
/////////////////////////////////////////////

class SBillingPlayer : public Player {
public:
    // constructor & destructor
    SBillingPlayer() throw(Error);
    SBillingPlayer(Socket* pSocket) throw(Error);

    virtual ~SBillingPlayer() throw(Error);

public:
    // network read / write
    virtual void processInput() throw(IOException, Error);
    virtual void processOutput() throw(IOException, Error);

    // parse packet and execute handler for the packet
    virtual void processCommand() throw(IOException, Error);

    // send packet to player's output buffer
    virtual void sendPacket(Packet* pPacket) throw(ProtocolException, Error);

    // disconnect
    
    
    
    virtual void disconnect(bool bDisconnected = DISCONNECTED) throw(InvalidProtocolException, Error);

    void setSocket(Socket* pSocket) throw();

    // get debug string
    virtual string toString() const throw(Error);

public:
    void sendIntervalValidation() throw(ProtocolException, Error);
    void sendLogin(PayUser* pPayUser) throw(ProtocolException, Error);
    void sendMinusPoint(PayUser* pPayUser) throw(ProtocolException, Error);
    void sendMinusMinute(PayUser* pPayUser) throw(ProtocolException, Error);
    void sendLogout(PayUser* pPayUser) throw(ProtocolException, Error);

public:
    
    void executeError(CBillingPacketHeader& header, CBillingPacketErrorBody& body) throw(IOException, Error);
    void executeIntervalValidation(CBillingPacketHeader& header,
                                   CBillingPacketResponseIntervalValidationBody& body) throw(IOException, Error);
    void executeLogin(CBillingPacketHeader& header, CBillingPacketResponseLoginBody& body) throw(IOException, Error);
    void executeMinusPoint(CBillingPacketHeader& header, CBillingPacketResponseMinusPointBody& body) throw(IOException,
                                                                                                           Error);
    void executeMinusMinute(CBillingPacketHeader& header,
                            CBillingPacketResponseMinusMinuteBody& body) throw(IOException, Error);
    void executeLogout(CBillingPacketHeader& header, CBillingPacketResponseLogoutBody& body) throw(IOException, Error);

public:
};

#endif
