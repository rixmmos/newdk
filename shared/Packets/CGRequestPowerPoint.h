//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPowerPoint.h
// Written By  : bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_POWER_POINT_H__
#define __CG_REQUEST_POWER_POINT_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGRequestPowerPoint;
//////////////////////////////////////////////////////////////////////////////

class CGRequestPowerPoint : public Packet {
public:
    CGRequestPowerPoint();
    ~CGRequestPowerPoint();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_REQUEST_POWER_POINT;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE + m_CellNum.size();
    }
    string getPacketName() const {
        return "CGRequestPowerPoint";
    }
    string toString() const;

public:
    // get / set cell number
    const string& getCellNum() const {
        return m_CellNum;
    }
    void setCellNum(const string& cellnum) {
        m_CellNum = cellnum;
    }

protected:
    string m_CellNum;
};

//////////////////////////////////////////////////////////////////////////////
// class CGRequestPowerPointFactory;
//////////////////////////////////////////////////////////////////////////////

class CGRequestPowerPointFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGRequestPowerPoint();
    }
    string getPacketName() const throw() {
        return "CGRequestPowerPoint";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_REQUEST_POWER_POINT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 12;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGRequestPowerPointHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGRequestPowerPointHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGRequestPowerPointHandler {
public:
    static void execute(CGRequestPowerPoint* pCGRequestPowerPoint, Player* pPlayer);
};
#endif


#endif
