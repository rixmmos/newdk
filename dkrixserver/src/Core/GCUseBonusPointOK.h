//////////////////////////////////////////////////////////////////////
//
// Filename    : GCUseBonusPointOK.h
// Written By  : crazydog

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_USE_BONUS_POINT_OK_H__
#define __GC_USE_BONUS_POINT_OK_H__

// include files
#include "ModifyInfo.h"
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCUseBonusPointOK;
//
//////////////////////////////////////////////////////////////////////

class GCUseBonusPointOK : public ModifyInfo {
public:
    // Constructor
    GCUseBonusPointOK();

    // Desctructor
    ~GCUseBonusPointOK();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_USE_BONUS_POINT_OK;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return ModifyInfo::getPacketSize();
    }

    // get packet name
    string getPacketName() const {
        return "GCUseBonusPointOK";
    }

    // get packet's debug string
    string toString() const;
};


//////////////////////////////////////////////////////////////////////
//
// class GCUseBonusPointOKFactory;
//
// Factory for GCUseBonusPointOK
//
//////////////////////////////////////////////////////////////////////

class GCUseBonusPointOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCUseBonusPointOK();
    }

    // get packet name
    string getPacketName() const {
        return "GCUseBonusPointOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_USE_BONUS_POINT_OK;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return ModifyInfo::getPacketMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCUseBonusPointOKHandler;
//
//////////////////////////////////////////////////////////////////////

class GCUseBonusPointOKHandler {
public:
    // execute packet's handler
    static void execute(GCUseBonusPointOK* pPacket, Player* player);
};

#endif
