//////////////////////////////////////////////////////////////////////
//
// Filename    : CGUseBonusPoint.h
// Written By  : crazydog
// Description : vampire bonus point packet.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_BONUS_POINT_H__
#define __CG_USE_BONUS_POINT_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

#define INC_INT 0
#define INC_STR 1
#define INC_DEX 2

class CGUseBonusPoint : public Packet {
public:
    CGUseBonusPoint() throw();
    ~CGUseBonusPoint() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_USE_BONUS_POINT;
    }
    PacketSize_t getPacketSize() const throw() {
        return szBYTE;
    }
    string getPacketName() const throw() {
        return "CGUseBonusPoint";
    }
    BYTE getWhich() const throw() {
        return m_Which;
    }
    void setWhich(BYTE which) throw() {
        m_Which = which;
    }
    string toString() const throw();

private:
    BYTE m_Which;
};

class CGUseBonusPointFactory : public PacketFactory {
public:
    CGUseBonusPointFactory() throw() {}
    virtual ~CGUseBonusPointFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGUseBonusPoint();
    }
    string getPacketName() const throw() {
        return "CGUseBonusPoint";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_USE_BONUS_POINT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE;
    }
};

class CGUseBonusPointHandler {
public:
    static void execute(CGUseBonusPoint* pCGUseBonusPoint, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
