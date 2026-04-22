//////////////////////////////////////////////////////////////////////
// Filename    : CGRideMotorCycle.h
//////////////////////////////////////////////////////////////////////

#ifndef __CG_RIDE_MOTORCYCLE_H__
#define __CG_RIDE_MOTORCYCLE_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGRideMotorCycle : public Packet {
public:
    CGRideMotorCycle() throw();
    ~CGRideMotorCycle() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_RIDE_MOTORCYCLE;
    }

    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szCoord + szCoord;
    }

    std::string getPacketName() const throw() {
        return "CGRideMotorCycle";
    }

    std::string toString() const throw();

    ObjectID_t getObjectID() const throw() {
        return m_ObjectID;
    }

    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

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

private:
    ObjectID_t m_ObjectID;
    Coord_t m_X;
    Coord_t m_Y;
};

class CGRideMotorCycleFactory : public PacketFactory {
public:
    CGRideMotorCycleFactory() throw() {}
    virtual ~CGRideMotorCycleFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGRideMotorCycle();
    }

    std::string getPacketName() const throw() {
        return "CGRideMotorCycle";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_RIDE_MOTORCYCLE;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoord + szCoord;
    }
};

class CGRideMotorCycleHandler {
public:
    static void execute(CGRideMotorCycle* pPacket, Player* pPlayer);
};

#endif
