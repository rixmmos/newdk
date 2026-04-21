//////////////////////////////////////////////////////////////////////
//
// Filename    : CGUsePotionFromQuickSlot.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_POTION_FROM_QUICKSLOT_H__
#define __CG_USE_POTION_FROM_QUICKSLOT_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGUsePotionFromQuickSlot : public Packet {
public:
    CGUsePotionFromQuickSlot() throw();
    ~CGUsePotionFromQuickSlot() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_USE_POTION_FROM_QUICKSLOT;
    }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szSlotID;
    }
    string getPacketName() const throw() {
        return "CGUsePotionFromQuickSlot";
    }
    string toString() const throw();

    ObjectID_t getObjectID() const throw() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

    SlotID_t getSlotID() const throw() {
        return m_SlotID;
    }
    void setSlotID(SlotID_t slotID) throw() {
        m_SlotID = slotID;
    }

private:
    ObjectID_t m_ObjectID;
    SlotID_t m_SlotID;
};

class CGUsePotionFromQuickSlotFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGUsePotionFromQuickSlot();
    }
    string getPacketName() const throw() {
        return "CGUsePotionFromQuickSlot";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_USE_POTION_FROM_QUICKSLOT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szSlotID;
    }
};

class CGUsePotionFromQuickSlotHandler {
public:
    static void execute(CGUsePotionFromQuickSlot* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
