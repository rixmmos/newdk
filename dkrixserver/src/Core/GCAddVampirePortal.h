//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddVampirePortal.h
// Written By  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_VAMPIRE_PORTAL_H__
#define __GC_ADD_VAMPIRE_PORTAL_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddVampirePortal;
//////////////////////////////////////////////////////////////////////////////

class GCAddVampirePortal : public Packet {
public:
    GCAddVampirePortal() {};
    ~GCAddVampirePortal() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_ADD_VAMPIRE_PORTAL;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE + m_OwnerID.size() + szDuration + szCoord * 2 + szZoneID + szCoord * 2 + szBYTE;
    }
    string getPacketName() const {
        return "GCAddVampirePortal";
    }
    string toString() const;

public:
    ObjectID_t getObjectID(void) const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t d) {
        m_ObjectID = d;
    }

    string getOwnerID(void) const {
        return m_OwnerID;
    }
    void setOwnerID(string ownerID) {
        m_OwnerID = ownerID;
    }

    Duration_t getDuration() const {
        return m_Duration;
    }
    void setDuration(Duration_t d) {
        m_Duration = d;
    }

    Coord_t getX(void) const {
        return m_X;
    }
    void setX(Coord_t x) {
        m_X = x;
    }

    Coord_t getY(void) const {
        return m_Y;
    }
    void setY(Coord_t x) {
        m_Y = x;
    }

    ZoneID_t getTargetZoneID(void) const {
        return m_TargetZoneID;
    }
    void setTargetZoneID(ZoneID_t id) {
        m_TargetZoneID = id;
    }

    Coord_t getTargetX(void) const {
        return m_TargetX;
    }
    void setTargetX(Coord_t x) {
        m_TargetX = x;
    }

    Coord_t getTargetY(void) const {
        return m_TargetY;
    }
    void setTargetY(Coord_t x) {
        m_TargetY = x;
    }

    BYTE getCreateFlag(void) const {
        return m_CreateFlag;
    }
    void setCreateFlag(BYTE flag) {
        m_CreateFlag = flag;
    }


private:
    ObjectID_t m_ObjectID;   
    string m_OwnerID;        
    Duration_t m_Duration;   
    Coord_t m_X;             
    Coord_t m_Y;             
    ZoneID_t m_TargetZoneID; 
    Coord_t m_TargetX;       
    Coord_t m_TargetY;       
    BYTE m_CreateFlag;       
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddVampirePortalFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddVampirePortalFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCAddVampirePortal();
    }
    string getPacketName() const {
        return "GCAddVampirePortal";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ADD_VAMPIRE_PORTAL;
    }
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szBYTE + 20 + szDuration + szCoord * 2 + szZoneID + szCoord * 2 + szBYTE;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddVampirePortalHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAddVampirePortalHandler {
public:
    static void execute(GCAddVampirePortal* pGCAddVampirePortal, Player* pPlayer);
};

#endif
