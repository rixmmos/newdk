//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddZoneToMouse.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_ZONE_TO_MOUSE_H__
#define __CG_ADD_ZONE_TO_MOUSE_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGAddZoneToMouse;
//////////////////////////////////////////////////////////////////////////////

class CGAddZoneToMouse : public Packet {
public:
    CGAddZoneToMouse();
    ~CGAddZoneToMouse();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_ADD_ZONE_TO_MOUSE;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szCoord + szCoord;
    }
    string getPacketName() const {
        return "CGAddZoneToMouse";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    Coord_t getZoneX() const {
        return m_ZoneX;
    }
    void setZoneX(Coord_t ZoneX) {
        m_ZoneX = ZoneX;
    }

    Coord_t getZoneY() const {
        return m_ZoneY;
    }
    void setZoneY(Coord_t ZoneY) {
        m_ZoneY = ZoneY;
    }

private:
    ObjectID_t m_ObjectID;

    Coord_t m_ZoneX;
    Coord_t m_ZoneY;
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddZoneToMouseFactory;
//////////////////////////////////////////////////////////////////////////////

class CGAddZoneToMouseFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGAddZoneToMouse();
    }
    string getPacketName() const throw() {
        return "CGAddZoneToMouse";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_ADD_ZONE_TO_MOUSE;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoord + szCoord;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddZoneToMouseHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGAddZoneToMouseHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGAddZoneToMouseHandler {
public:
    static void execute(CGAddZoneToMouse* pPacket, Player* player);
};
#endif

#endif
