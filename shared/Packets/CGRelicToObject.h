//////////////////////////////////////////////////////////////////////
//
// Filename    : CGRelicToObject.h
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_RELIC_TO_OBJECT_H__
#define __CG_RELIC_TO_OBJECT_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGRelicToObject : public Packet {
public:
    CGRelicToObject() throw();
    ~CGRelicToObject() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_RELIC_TO_OBJECT;
    }

    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szObjectID + szCoord + szCoord;
    }

    std::string getPacketName() const throw() {
        return "CGRelicToObject";
    }

    std::string toString() const throw();

    Coord_t getX() const throw() {
        return m_X;
    }

    void setX(Coord_t x) throw() {
        m_X = x;
    }

    Coord_t getY() const throw() {
        return m_Y;
    }

    void setY(Coord_t y) throw() {
        m_Y = y;
    }

    ObjectID_t getObjectID() const throw() {
        return m_ObjectID;
    }

    ObjectID_t getItemObjectID() const throw() {
        return m_ItemObjectID;
    }

    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

    void setItemObjectID(ObjectID_t itemObjectID) throw() {
        m_ItemObjectID = itemObjectID;
    }

private:
    ObjectID_t m_ItemObjectID;
    ObjectID_t m_ObjectID;
    Coord_t m_X;
    Coord_t m_Y;
};

class CGRelicToObjectFactory : public PacketFactory {
public:
    CGRelicToObjectFactory() throw() {}
    virtual ~CGRelicToObjectFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGRelicToObject();
    }

    std::string getPacketName() const throw() {
        return "CGRelicToObject";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_RELIC_TO_OBJECT;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szObjectID + szCoord + szCoord;
    }
};

#ifndef __GAME_CLIENT__
class CGRelicToObjectHandler {
public:
    static void execute(CGRelicToObject* pPacket, Player* pPlayer);

private:
    static void executeRelic(CGRelicToObject* pPacket, Player* pPlayer);
    static void executeBloodBible(CGRelicToObject* pPacket, Player* pPlayer);
    static void executeCastleSymbol(CGRelicToObject* pPacket, Player* pPlayer);
    static void executeFlag(CGRelicToObject* pPacket, Player* pPlayer);
    static void executeSweeper(CGRelicToObject* pPacket, Player* pPlayer);
};
#endif

#endif
