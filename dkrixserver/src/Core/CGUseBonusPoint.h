//////////////////////////////////////////////////////////////////////
//
// Filename    : CGUseBonusPoint.h
// Written By  : crazydog

//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_BONUS_POINT_H__
#define __CG_USE_BONUS_POINT_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"


#define INC_INT 0
#define INC_STR 1
#define INC_DEX 2


//////////////////////////////////////////////////////////////////////
//
// class CGUseBonusPoint;
//
//////////////////////////////////////////////////////////////////////

class CGUseBonusPoint : public Packet {
public:
    // constructor
    CGUseBonusPoint();

    // destructor
    ~CGUseBonusPoint();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_USE_BONUS_POINT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE;
    }

    // get packet name
    string getPacketName() const {
        return "CGUseBonusPoint";
    }

    // get/set which
    BYTE getWhich() const {
        return m_Which;
    }
    void setWhich(BYTE w) {
        m_Which = w;
    }

    // get packet's debug string
    string toString() const;

private:
    // which
    BYTE m_Which;
};


//////////////////////////////////////////////////////////////////////
//
// class CGUseBonusPointFactory;
//
// Factory for CGUseBonusPoint
//
//////////////////////////////////////////////////////////////////////

class CGUseBonusPointFactory : public PacketFactory {
public:
    // constructor
    CGUseBonusPointFactory() {}

    // destructor
    virtual ~CGUseBonusPointFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new CGUseBonusPoint();
    }

    // get packet name
    string getPacketName() const {
        return "CGUseBonusPoint";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_USE_BONUS_POINT;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGUseBonusPointHandler;
//
//////////////////////////////////////////////////////////////////////

class CGUseBonusPointHandler {
public:
    // execute packet's handler
    static void execute(CGUseBonusPoint* pCGUseBonusPoint, Player* pPlayer);
};

#endif
