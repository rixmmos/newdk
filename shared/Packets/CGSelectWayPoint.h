//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectWayPoint.h
// Written By  : excel96
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_WAYPOINT_H__
#define __CG_SELECT_WAYPOINT_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectWayPoint;
//////////////////////////////////////////////////////////////////////////////

class CGSelectWayPoint : public Packet {
public:
    CGSelectWayPoint() {};
    virtual ~CGSelectWayPoint() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_WAYPOINT;
    }
    PacketSize_t getPacketSize() const {
        return szZoneID + szCoord * 2;
    }
    string getPacketName() const {
        return "CGSelectWayPoint";
    }
    string toString() const;

public:
    ZoneID_t getZoneID() const {
        return m_ZoneID;
    }
    void setZoneID(ZoneID_t ZoneID) {
        m_ZoneID = ZoneID;
    }

    Coord_t getX(void) const {
        return m_X;
    }
    void setX(Coord_t X) {
        m_X = X;
    }

    Coord_t getY(void) const {
        return m_Y;
    }
    void setY(Coord_t Y) {
        m_Y = Y;
    }

private:
    ZoneID_t m_ZoneID; 
    Coord_t m_X;       
    Coord_t m_Y;       
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectWayPointFactory;
//////////////////////////////////////////////////////////////////////////////

class CGSelectWayPointFactory : public PacketFactory {
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGSelectWayPoint();
    }
    string getPacketName() const throw() {
        return "CGSelectWayPoint";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SELECT_WAYPOINT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szZoneID + szCoord * 2;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectWayPointHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGSelectWayPointHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGSelectWayPointHandler {
public:
    static void execute(CGSelectWayPoint* pCGSelectWayPoint, Player* pPlayer);
    static void executeEnterQuestZone(CGSelectWayPoint* pCGSelectWayPoint, Player* pPlayer, int targetDynamicZoneType);
};
#endif

#endif
