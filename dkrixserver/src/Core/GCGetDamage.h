//////////////////////////////////////////////////////////////////////
//
// Filename    :  GCGetDamage.h
// Written By  :  elca@ewestsoft.com



//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_GET_DAMAGE_H__
#define __GC_GET_DAMAGE_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class  GCGetDamage;
//


//
//////////////////////////////////////////////////////////////////////

class GCGetDamage : public Packet {
public:
    // constructor
    GCGetDamage();

    // destructor
    ~GCGetDamage();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_GET_DAMAGE;
    }

    // get packet size
    PacketSize_t getPacketSize() const {
        return szObjectID + szWORD;
    }

    // get packet's name
    string getPacketName() const {
        return "GCGetDamage";
    }

    // get packet's debug string
    string toString() const;

    // get/set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    // get/set Damage
    WORD getDamage() const {
        return m_GetDamage;
    }
    void setDamage(WORD GetDamage) {
        m_GetDamage = GetDamage;
    }

private:
    ObjectID_t m_ObjectID; // ObjectID..
    WORD m_GetDamage;      // Damage..
};


//////////////////////////////////////////////////////////////////////
//
// class  GCGetDamageFactory;
//
// Factory for  GCGetDamage
//
//////////////////////////////////////////////////////////////////////

class GCGetDamageFactory : public PacketFactory {
public:
    // constructor
    GCGetDamageFactory() {}

    // destructor
    virtual ~GCGetDamageFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCGetDamage();
    }

    // get packet name
    string getPacketName() const {
        return "GCGetDamage";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_GET_DAMAGE;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szWORD;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class  GCGetDamageHandler;
//
//////////////////////////////////////////////////////////////////////

class GCGetDamageHandler {
public:
    // execute packet's handler
    static void execute(GCGetDamage* pGCGetDamage, Player* pPlayer);
};

#endif
