//////////////////////////////////////////////////////////////////////////////
// Filename    : CGGQuestAccept.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_GQUEST_ACCEPT_H__
#define __CG_GQUEST_ACCEPT_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGGQuestAccept;
//////////////////////////////////////////////////////////////////////////////

class CGGQuestAccept : public Packet {
public:
    CGGQuestAccept() {};
    ~CGGQuestAccept() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_GQUEST_ACCEPT;
    }
    PacketSize_t getPacketSize() const {
        return szDWORD;
    }
    string getPacketName() const {
        return "CGGQuestAccept";
    }
    string toString() const;

public:
    DWORD getQuestID() const {
        return m_QuestID;
    }
    void setQuestID(DWORD QuestID) {
        m_QuestID = QuestID;
    }

private:
    DWORD m_QuestID; 
};

//////////////////////////////////////////////////////////////////////
// class CGGQuestAcceptFactory;
//////////////////////////////////////////////////////////////////////

class CGGQuestAcceptFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGGQuestAccept();
    }
    string getPacketName() const throw() {
        return "CGGQuestAccept";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_GQUEST_ACCEPT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szDWORD;
    }
};


//////////////////////////////////////////////////////////////////////
// class CGGQuestAcceptHandler;
//////////////////////////////////////////////////////////////////////

// Server-only: CGGQuestAcceptHandler::execute has no client-side
// definition or use. No CGHandlersStub.cpp-style client stub exists for
// this family, but unlike CGAuthKey the client's own pre-migration copy
// left this class declaration itself unguarded (only the .cpp's dispatch
// call is guarded) — a static method that is declared but never
// ODR-used under __GAME_CLIENT__ needs no definition, so this matches
// the client tree's existing behavior exactly.
class CGGQuestAcceptHandler {
public:
    static void execute(CGGQuestAccept* pCGGQuestAccept, Player* pPlayer);
};

#endif
