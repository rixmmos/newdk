//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSelectNickname.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_NICKNAME_H__
#define __CG_SELECT_NICKNAME_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSelectNickname;
//
//////////////////////////////////////////////////////////////////////

class CGSelectNickname : public Packet {
public:
    CGSelectNickname() {};
    virtual ~CGSelectNickname() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_NICKNAME;
    }
    PacketSize_t getPacketSize() const {
        return szWORD;
    }
    string getPacketName() const {
        return "CGSelectNickname";
    }
    string toString() const;

    WORD getNicknameID() const {
        return m_NicknameID;
    }
    void setNicknameID(WORD NicknameID) {
        m_NicknameID = NicknameID;
    }

private:
    WORD m_NicknameID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectNicknameFactory;
//
// Factory for CGSelectNickname
//
//////////////////////////////////////////////////////////////////////

class CGSelectNicknameFactory : public PacketFactory {
public:
    CGSelectNicknameFactory() {}
    virtual ~CGSelectNicknameFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGSelectNickname();
    }
    string getPacketName() const throw() {
        return "CGSelectNickname";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SELECT_NICKNAME;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szWORD;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectNicknameHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGSelectNicknameHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGSelectNicknameHandler {
public:
    // execute packet's handler
    static void execute(CGSelectNickname* pCGSelectNickname, Player* pPlayer);
};
#endif

#endif
