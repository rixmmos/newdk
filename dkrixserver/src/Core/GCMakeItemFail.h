//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMakeItemFail.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MAKE_ITEM_FAIL_H__
#define __GC_MAKE_ITEM_FAIL_H__

// include files
#include "Exception.h"
#include "GCChangeInventoryItemNum.h"
#include "ModifyInfo.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemFail;
//

//
//////////////////////////////////////////////////////////////////////

class GCMakeItemFail : public GCChangeInventoryItemNum, public ModifyInfo {
public:
    // constructor
    GCMakeItemFail();

    // destructor
    ~GCMakeItemFail();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_MAKE_ITEM_FAIL;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return GCChangeInventoryItemNum::getPacketSize() + ModifyInfo::getPacketSize();
    }

    // get packet's name
    string getPacketName() const {
        return "GCMakeItemFail";
    }

    // get packet's debug string
    string toString() const;

private:
};


//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemFailFactory;
//
// Factory for GCMakeItemFail
//
//////////////////////////////////////////////////////////////////////

class GCMakeItemFailFactory : public PacketFactory {
public:
    // constructor
    GCMakeItemFailFactory() {}

    // destructor
    virtual ~GCMakeItemFailFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCMakeItemFail();
    }

    // get packet name
    string getPacketName() const {
        return "GCMakeItemFail";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_MAKE_ITEM_FAIL;
    }

    // get Packet Max Size
    // PacketSize_t getPacketMaxSize() const  { return szSkillType + szCEffectID + szDuration + szBYTE + szBYTE*
    // m_ListNum* 2 ; }
    PacketSize_t getPacketMaxSize() const {
        return 255 + ModifyInfo::getPacketMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemFailHandler;
//
//////////////////////////////////////////////////////////////////////

class GCMakeItemFailHandler {
public:
    // execute packet's handler
    static void execute(GCMakeItemFail* pGCMakeItemFail, Player* pPlayer);
};

#endif
