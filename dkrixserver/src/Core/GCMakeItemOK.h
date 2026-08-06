//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMakeItemOK.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MAKE_ITEM_OK_H__
#define __GC_MAKE_ITEM_OK_H__

// include files
#include "Exception.h"
#include "GCAddItemToInventory.h"
#include "GCChangeInventoryItemNum.h"
#include "ModifyInfo.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemOK;
//

//
//////////////////////////////////////////////////////////////////////

class GCMakeItemOK : public GCChangeInventoryItemNum, public GCAddItemToInventory, public ModifyInfo {
public:
    // constructor
    GCMakeItemOK();

    // destructor
    ~GCMakeItemOK();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_MAKE_ITEM_OK;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return GCChangeInventoryItemNum::getPacketSize() + GCAddItemToInventory::getPacketSize() +
               ModifyInfo::getPacketSize();
    }

    // get packet's name
    string getPacketName() const {
        return "GCMakeItemOK";
    }

    // get packet's debug string
    string toString() const;

private:
};


//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemOKFactory;
//
// Factory for GCMakeItemOK
//
//////////////////////////////////////////////////////////////////////

class GCMakeItemOKFactory : public PacketFactory {
public:
    // constructor
    GCMakeItemOKFactory() {}

    // destructor
    virtual ~GCMakeItemOKFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCMakeItemOK();
    }

    // get packet name
    string getPacketName() const {
        return "GCMakeItemOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_MAKE_ITEM_OK;
    }

    // get Packet Max Size
    // PacketSize_t getPacketMaxSize() const  { return szSkillType + szCEffectID + szDuration + szBYTE + szBYTE*
    // m_ListNum* 2 ; }
    PacketSize_t getPacketMaxSize() const {
        return 255 + 255 + ModifyInfo::getPacketMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemOKHandler;
//
//////////////////////////////////////////////////////////////////////

class GCMakeItemOKHandler {
public:
    // execute packet's handler
    static void execute(GCMakeItemOK* pGCMakeItemOK, Player* pPlayer);
};

#endif
