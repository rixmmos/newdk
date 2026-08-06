//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTakeOff.h
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_TAKE_OFF_H__
#define __GC_TAKE_OFF_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCTakeOff;
//


//
//////////////////////////////////////////////////////////////////////

class GCTakeOff : public Packet {
public:
    // constructor
    GCTakeOff();

    // destructor
    ~GCTakeOff();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_TAKE_OFF;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szSlotID;
    }

    // get packet's name
    string getPacketName() const {
        return "GCTakeOff";
    }

    // get packet's debug string
    string toString() const;

    // get / set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    // get/set SlotID
    SlotID_t getSlotID() const {
        return m_SlotID;
    }
    void setSlotID(SlotID_t SlotID) {
        m_SlotID = SlotID;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;

    // SlotID
    SlotID_t m_SlotID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCTakeOffFactory;
//
// Factory for GCTakeOff
//
//////////////////////////////////////////////////////////////////////

class GCTakeOffFactory : public PacketFactory {
public:
    // constructor
    GCTakeOffFactory() {}

    // destructor
    virtual ~GCTakeOffFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCTakeOff();
    }

    // get packet name
    string getPacketName() const {
        return "GCTakeOff";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_TAKE_OFF;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szSlotID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCTakeOffHandler;
//
//////////////////////////////////////////////////////////////////////

class GCTakeOffHandler {
public:
    // execute packet's handler
    static void execute(GCTakeOff* pGCTakeOff, Player* pPlayer);
};

#endif
