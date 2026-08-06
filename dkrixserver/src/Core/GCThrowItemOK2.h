//////////////////////////////////////////////////////////////////////
//
// Filename    : GCThrowItemOK2.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_THROW_ITEM_OK_2_H__
#define __GC_THROW_ITEM_OK_2_H__

// include files
#include "Exception.h"
#include "ModifyInfo.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK2 : public ModifyInfo {
public:
    // constructor
    GCThrowItemOK2();

    // destructor
    ~GCThrowItemOK2();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_THROW_ITEM_OK_2;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + ModifyInfo::getPacketSize();
    }

    // get packet's name
    string getPacketName() const {
        return "GCThrowItemOK2";
    }

    // get packet's debug string
    string toString() const;

    // get / set TargetObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

private:
    // TargetObjectID
    ObjectID_t m_ObjectID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK2Factory;
//
// Factory for GCThrowItemOK2
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK2Factory : public PacketFactory {
public:
    // constructor
    GCThrowItemOK2Factory() {}

    // destructor
    virtual ~GCThrowItemOK2Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCThrowItemOK2();
    }

    // get packet name
    string getPacketName() const {
        return "GCThrowItemOK2";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_THROW_ITEM_OK_2;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + ModifyInfo::getPacketMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCThrowItemOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCThrowItemOK2Handler {
public:
    // execute packet's handler
    static void execute(GCThrowItemOK2* pGCThrowItemOK2, Player* pPlayer);
};

#endif
