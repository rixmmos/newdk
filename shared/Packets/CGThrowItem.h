//////////////////////////////////////////////////////////////////////
// Filename    : CGThrowItem.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_THROW_ITEM_H__
#define __CG_THROW_ITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGThrowItem : public Packet {
public:
    CGThrowItem() throw();
    ~CGThrowItem() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_THROW_ITEM;
    }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szObjectID + szCoordInven + szCoordInven;
    }
    string getPacketName() const throw() {
        return "CGThrowItem";
    }
    string toString() const throw();

    ObjectID_t getObjectID() const throw() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

    ObjectID_t getTargetObjectID() const throw() {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t targetObjectID) throw() {
        m_TargetObjectID = targetObjectID;
    }

    CoordInven_t getX() const throw() {
        return m_InvenX;
    }
    void setX(CoordInven_t invenX) throw() {
        m_InvenX = invenX;
    }

    CoordInven_t getY() const throw() {
        return m_InvenY;
    }
    void setY(CoordInven_t invenY) throw() {
        m_InvenY = invenY;
    }

private:
    ObjectID_t m_ObjectID;
    ObjectID_t m_TargetObjectID;
    CoordInven_t m_InvenX;
    CoordInven_t m_InvenY;
};

class CGThrowItemFactory : public PacketFactory {
public:
    CGThrowItemFactory() throw() {}
    virtual ~CGThrowItemFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGThrowItem();
    }
    string getPacketName() const throw() {
        return "CGThrowItem";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_THROW_ITEM;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szObjectID + szCoordInven + szCoordInven;
    }
};

class CGThrowItemHandler {
public:
    static void execute(CGThrowItem* pCGThrowItem, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
