//////////////////////////////////////////////////////////////////////
//
// Filename    : CGRequestStoreInfo.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_STORE_INFO_H__
#define __CG_REQUEST_STORE_INFO_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGRequestStoreInfo;
//
//////////////////////////////////////////////////////////////////////

class CGRequestStoreInfo : public Packet {
public:
    CGRequestStoreInfo() {};
    virtual ~CGRequestStoreInfo() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_REQUEST_STORE_INFO;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet name
    string getPacketName() const {
        return "CGRequestStoreInfo";
    }

    // get packet's debug string
    string toString() const;

    ObjectID_t getOwnerObjectID() const {
        return m_OwnerObjectID;
    }
    void setOwnerObjectID(ObjectID_t oid) {
        m_OwnerObjectID = oid;
    }

private:
    ObjectID_t m_OwnerObjectID; 
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestStoreInfoFactory;
//
// Factory for CGRequestStoreInfo
//
//////////////////////////////////////////////////////////////////////

class CGRequestStoreInfoFactory : public PacketFactory {
public:
    // constructor
    CGRequestStoreInfoFactory() {}

    // destructor
    virtual ~CGRequestStoreInfoFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGRequestStoreInfo();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGRequestStoreInfo";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_REQUEST_STORE_INFO;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestStoreInfoHandler;
//
//////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGRequestStoreInfoHandler::execute (the "CGStoreOpen precedent").
// Keeping it unconditional here means CGHandlersStub.cpp needs no
// structural change — only its #include and exception spec were updated.
class CGRequestStoreInfoHandler {
public:
    // execute packet's handler
    static void execute(CGRequestStoreInfo* pCGRequestStoreInfo, Player* pPlayer);
};

#endif
