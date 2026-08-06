////////////////////////////////////////////////////////////////////////////////
// Filename    : CGStashList.h

// Description :


////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_STASH_LIST_H__
#define __CG_STASH_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashList;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashList : public Packet {
public:
    CGStashList() {};
    virtual ~CGStashList() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_STASH_LIST;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }
    string getPacketName() const {
        return "CGStashList";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t id) {
        m_ObjectID = id;
    }

private:
    ObjectID_t m_ObjectID; 
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashListFactory;
//
// Factory for CGStashList
//
////////////////////////////////////////////////////////////////////////////////

class CGStashListFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new CGStashList();
    }
    string getPacketName() const {
        return "CGStashList";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_STASH_LIST;
    }
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashListHandler;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashListHandler {
public:
    static void execute(CGStashList* pPacket, Player* player);
};

#endif
