//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRemoveEffect.h
// Written By  : crazydog

//
//////////////////////////////////////////////////////////////////////

#ifndef __REMOVE_EFFECT_H__
#define __REMOVE_EFFECT_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRemoveEffect;
//


//
//////////////////////////////////////////////////////////////////////

class GCRemoveEffect : public Packet {
public:
    // constructor
    GCRemoveEffect();

    // destructor
    ~GCRemoveEffect();

public:
    PacketID_t getPacketID() const {
        return PACKET_GC_REMOVE_EFFECT;
    }
    string getPacketName() const {
        return "GCRemoveEffect";
    }


    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;
    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE + szEffectID * m_ListNum;
    }
    static PacketSize_t getPacketMaxSize() {
        return 255;
    }

    // get packet's debug string
    string toString() const;

    // get / set ListNumber
    BYTE getListNum() const {
        return m_ListNum;
    }
    void setListNum(BYTE ListNum) {
        m_ListNum = ListNum;
    }

    // get&set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t id) {
        m_ObjectID = id;
    }

    // add / delete / clear S List
    void addEffectList(EffectID_t id);

    // ClearList
    void clearList() {
        m_EffectList.clear();
        m_ListNum = 0;
    }

    // pop front Element in Status List
    WORD popFrontListElement() {
        EffectID_t effectID = m_EffectList.front();
        m_EffectList.pop_front();
        return effectID;
    }

protected:
    ObjectID_t m_ObjectID;
    // StatusList Element Number
    BYTE m_ListNum;

    // Status List
    list<EffectID_t> m_EffectList;
};

//////////////////////////////////////////////////////////////////////
//
// class GCRemoveEffectFactory;
//
// Factory for GCRemoveEffect
//
//////////////////////////////////////////////////////////////////////

class GCRemoveEffectFactory : public PacketFactory {
public:
    // constructor
    GCRemoveEffectFactory() {}

    // destructor
    virtual ~GCRemoveEffectFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCRemoveEffect();
    }

    // get packet name
    string getPacketName() const {
        return "GCRemoveEffect";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_REMOVE_EFFECT;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return 255;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveEffectHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRemoveEffectHandler {
public:
    // execute packet's handler
    static void execute(GCRemoveEffect* pGCRemoveEffect, Player* pPlayer);
};


#endif
