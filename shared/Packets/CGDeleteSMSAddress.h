//////////////////////////////////////////////////////////////////////////////
// Filename    : CGDeleteSMSAddress.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_DELETE_SMS_ADDRESS_H__
#define __CG_DELETE_SMS_ADDRESS_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGDeleteSMSAddress;
//////////////////////////////////////////////////////////////////////////////

class CGDeleteSMSAddress : public Packet {
public:
    CGDeleteSMSAddress();
    ~CGDeleteSMSAddress();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_DELETE_SMS_ADDRESS;
    }
    PacketSize_t getPacketSize() const {
        return szDWORD;
    }
    string getPacketName() const {
        return "CGDeleteSMSAddress";
    }
    string toString() const;

public:
    DWORD getElementID() const {
        return m_ElementID;
    }
    void setElementID(DWORD eID) {
        m_ElementID = eID;
    }

private:
    DWORD m_ElementID;
};

//////////////////////////////////////////////////////////////////////////////
// class CGDeleteSMSAddressFactory;
//////////////////////////////////////////////////////////////////////////////

class CGDeleteSMSAddressFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGDeleteSMSAddress();
    }
    string getPacketName() const throw() {
        return "CGDeleteSMSAddress";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DELETE_SMS_ADDRESS;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szDWORD;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGDeleteSMSAddressHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGDeleteSMSAddressHandler::execute has no client-side
// definition or use. No CGHandlersStub.cpp-style client stub exists for
// this family, but unlike CGAuthKey the client's own pre-migration copy
// left this class declaration itself unguarded (only the .cpp's dispatch
// call is guarded) — a static method that is declared but never
// ODR-used under __GAME_CLIENT__ needs no definition, so this matches
// the client tree's existing behavior exactly.
class CGDeleteSMSAddressHandler {
public:
    static void execute(CGDeleteSMSAddress* pPacket, Player* player);
};

#endif
