//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddHelicopter.h
// Written By  : excel96
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_HELICOPTER_H__
#define __GC_ADD_HELICOPTER_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopter;
//////////////////////////////////////////////////////////////////////////////

class GCAddHelicopter : public Packet {
public:
    GCAddHelicopter() {};
    ~GCAddHelicopter() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_ADD_HELICOPTER;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE;
    }
    string getPacketName() const {
        return "GCAddHelicopter";
    }
    string toString() const;

public:
    ObjectID_t getObjectID(void) const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t d) {
        m_ObjectID = d;
    }

    BYTE setCode(void) const {
        return m_Code;
    }
    void setCode(BYTE code) {
        m_Code = code;
    }

private:
    ObjectID_t m_ObjectID; 
    BYTE m_Code;           
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopterFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddHelicopterFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCAddHelicopter();
    }
    string getPacketName() const {
        return "GCAddHelicopter";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ADD_HELICOPTER;
    }
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szBYTE;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopterHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAddHelicopterHandler {
public:
    static void execute(GCAddHelicopter* pGCAddHelicopter, Player* pPlayer);
};

#endif
