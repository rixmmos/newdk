////////////////////////////////////////////////////////////////////////////////
// Filename    : CGMouseToStash.h
// Written By  : 김성민
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_MOUSE_TO_STASH_H__
#define __CG_MOUSE_TO_STASH_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGMouseToStash
//
////////////////////////////////////////////////////////////////////////////////

class CGMouseToStash : public Packet {
public:
    CGMouseToStash() throw() {}
    ~CGMouseToStash() throw() {}
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_MOUSE_TO_STASH;
    }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szBYTE * 2;
    }
    string getPacketName() const {
        return "CGMouseToStash";
    }
    string toString() const;

public:
    ObjectID_t getObjectID(void) throw() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) throw() {
        m_ObjectID = ObjectID;
    }
    BYTE getRack(void) throw() {
        return m_Rack;
    }
    void setRack(BYTE rack) throw() {
        m_Rack = rack;
    }
    BYTE getIndex(void) throw() {
        return m_Index;
    }
    void setIndex(BYTE index) throw() {
        m_Index = index;
    }

private:
    ObjectID_t m_ObjectID;
    BYTE m_Rack;
    BYTE m_Index;
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGMouseToStashFactory;
//
////////////////////////////////////////////////////////////////////////////////

class CGMouseToStashFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGMouseToStash();
    }
    string getPacketName() const {
        return "CGMouseToStash";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_MOUSE_TO_STASH;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szBYTE * 2;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGMouseToStashHandler;
//
////////////////////////////////////////////////////////////////////////////////

class CGMouseToStashHandler {
public:
    static void execute(CGMouseToStash* pPacket, Player* player) throw(ProtocolException, Error);
    // static void executeSlayer(CGMouseToStash* pPacket, Player* player) ;
    // static void executeVampire(CGMouseToStash* pPacket, Player* player) ;
};


#endif
