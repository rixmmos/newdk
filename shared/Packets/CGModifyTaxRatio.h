//////////////////////////////////////////////////////////////////////
//
// Filename    : CGModifyTaxRatio.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_MODIFY_TAX_RATIO_H__
#define __CG_MODIFY_TAX_RATIO_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGModifyTaxRatio;
//
//////////////////////////////////////////////////////////////////////

class CGModifyTaxRatio : public Packet {
public:
    CGModifyTaxRatio() {};
    ~CGModifyTaxRatio() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_MODIFY_TAX_RATIO;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szuint;
    }

    // get packet name
    string getPacketName() const {
        return "CGModifyTaxRatio";
    }

    // get packet's debug string
    string toString() const;

    // get/set Ratio
    uint getRatio() const {
        return m_Ratio;
    }
    void setRatio(uint ratio) {
        m_Ratio = ratio;
    }


private:
    uint m_Ratio;
};


//////////////////////////////////////////////////////////////////////
//
// class CGModifyTaxRatioFactory;
//
// Factory for CGModifyTaxRatio
//
//////////////////////////////////////////////////////////////////////

class CGModifyTaxRatioFactory : public PacketFactory {
public:
    // constructor
    CGModifyTaxRatioFactory() {}

    // destructor
    virtual ~CGModifyTaxRatioFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGModifyTaxRatio();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGModifyTaxRatio";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_MODIFY_TAX_RATIO;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szuint;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGModifyTaxRatioHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGModifyTaxRatioHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGModifyTaxRatioHandler {
public:
    // execute packet's handler
    static void execute(CGModifyTaxRatio* pCGModifyTaxRatio, Player* pPlayer);
};
#endif

#endif
