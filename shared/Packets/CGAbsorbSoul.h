//////////////////////////////////////////////////////////////////////
// Filename    : CGAbsorbSoul.h
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ABSORB_SOUL_H__
#define __CG_ABSORB_SOUL_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGAbsorbSoul : public Packet {
public:
    CGAbsorbSoul() throw();
    ~CGAbsorbSoul() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() { return PACKET_CG_ABSORB_SOUL; }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szZoneCoord * 2 + szObjectID + szCoordInven * 4;
    }
    string getPacketName() const throw() { return "CGAbsorbSoul"; }
    string toString() const throw();

    ObjectID_t getObjectID() const throw() { return m_ObjectID; }
    void setObjectID(ObjectID_t objectID) throw() { m_ObjectID = objectID; }

    ObjectID_t getInvenObjectID() const throw() { return m_InvenObjectID; }
    void setInvenObjectID(ObjectID_t invenObjectID) throw() { m_InvenObjectID = invenObjectID; }

    Coord_t getInvenX() const throw() { return m_InvenX; }
    void setInvenX(CoordInven_t x) throw() { m_InvenX = x; }

    Coord_t getInvenY() const throw() { return m_InvenY; }
    void setInvenY(CoordInven_t y) throw() { m_InvenY = y; }

    Coord_t getTargetInvenX() const throw() { return m_TargetInvenX; }
    void setTargetInvenX(CoordInven_t x) throw() { m_TargetInvenX = x; }

    Coord_t getTargetInvenY() const throw() { return m_TargetInvenY; }
    void setTargetInvenY(CoordInven_t y) throw() { m_TargetInvenY = y; }

    Coord_t getTargetZoneX() const throw() { return m_TargetZoneX; }
    void setTargetZoneX(ZoneCoord_t x) throw() { m_TargetZoneX = x; }

    Coord_t getTargetZoneY() const throw() { return m_TargetZoneY; }
    void setTargetZoneY(ZoneCoord_t y) throw() { m_TargetZoneY = y; }

private:
    ObjectID_t m_ObjectID;
    ObjectID_t m_InvenObjectID;
    CoordInven_t m_InvenX;
    CoordInven_t m_InvenY;
    CoordInven_t m_TargetInvenX;
    CoordInven_t m_TargetInvenY;
    ZoneCoord_t m_TargetZoneX;
    ZoneCoord_t m_TargetZoneY;
};

class CGAbsorbSoulFactory : public PacketFactory {
public:
    CGAbsorbSoulFactory() throw() {}
    virtual ~CGAbsorbSoulFactory() throw() {}

    Packet* createPacket() throw() { return new CGAbsorbSoul(); }
    string getPacketName() const throw() { return "CGAbsorbSoul"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_ABSORB_SOUL; }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szZoneCoord * 2 + szObjectID + szCoordInven * 4;
    }
};

class CGAbsorbSoulHandler {
public:
    static void execute(CGAbsorbSoul* pCGAbsorbSoul, Player* player) throw(ProtocolException, Error);
};

#endif
