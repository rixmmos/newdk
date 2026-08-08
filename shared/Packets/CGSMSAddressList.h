//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSMSAddressList.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SMS_ADDRESS_LIST_H__
#define __CG_SMS_ADDRESS_LIST_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSMSAddressList;
//////////////////////////////////////////////////////////////////////////////

class CGSMSAddressList : public Packet {
public:
    CGSMSAddressList() {};
    virtual ~CGSMSAddressList() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_SMS_ADDRESS_LIST;
    }
    PacketSize_t getPacketSize() const {
        return 0;
    }
    string getPacketName() const {
        return "CGSMSAddressList";
    }
    string toString() const;
};

//////////////////////////////////////////////////////////////////////
// class CGSMSAddressListFactory;
//////////////////////////////////////////////////////////////////////

class CGSMSAddressListFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGSMSAddressList();
    }
    string getPacketName() const throw() {
        return "CGSMSAddressList";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SMS_ADDRESS_LIST;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
// class CGSMSAddressListHandler;
//////////////////////////////////////////////////////////////////////

// Server-only: CGSMSAddressListHandler::execute has no client-side
// definition or use. No CGHandlersStub.cpp-style client stub exists for
// this family, but unlike most of this batch the client's own
// pre-migration copy left this class declaration itself unguarded
// (only the .cpp's dispatch call is guarded) -- a static method that is
// declared but never ODR-used under __GAME_CLIENT__ needs no
// definition, so this matches the client tree's existing behavior
// exactly.
class CGSMSAddressListHandler {
public:
    static void execute(CGSMSAddressList* pCGSMSAddressList, Player* pPlayer);
};

#endif
