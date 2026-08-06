//////////////////////////////////////////////////////////////////////
//
// Filename    : GCBloodBibleStatus.h
// Written By  : Reiot
//
//////////////////////////////////////////////////////////////////////
//




//
//////////////////////////////////////////////////////////////////////


#ifndef __GC_BLOOD_BIBLE_STATUS_H__
#define __GC_BLOOD_BIBLE_STATUS_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCBloodBibleStatus;
//



//
//////////////////////////////////////////////////////////////////////

class GCBloodBibleStatus : public Packet {
public:
    GCBloodBibleStatus() {};
    ~GCBloodBibleStatus() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_BLOOD_BIBLE_STATUS;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szItemType + szZoneID + szStorage + szBYTE + m_OwnerName.size() + szRace + szRace + szZoneCoord +
               szZoneCoord;
    }

    // get packet name
    string getPacketName() const {
        return "GCBloodBibleStatus";
    }

    // get packet's debug string
    string toString() const;

    // get/set text color
    ItemType_t getItemType() const {
        return m_ItemType;
    }
    void setItemType(ItemType_t itemType) {
        m_ItemType = itemType;
    }

    // get/set text color
    ZoneID_t getZoneID() const {
        return m_ZoneID;
    }
    void setZoneID(ZoneID_t zoneID) {
        m_ZoneID = zoneID;
    }

    // get/set text color
    Storage_t getStorage() const {
        return m_Storage;
    }
    void setStorage(Storage_t storage) {
        m_Storage = storage;
    }

    // get/set chatting message
    const string& getOwnerName() const {
        return m_OwnerName;
    }
    void setOwnerName(const string& OwnerName) {
        m_OwnerName = OwnerName;
    }

    // get/set text color
    Race_t getRace() const {
        return m_Race;
    }
    void setRace(Race_t race) {
        m_Race = race;
    }

    // get/set text color
    Race_t getShrineRace() const {
        return m_ShrineRace;
    }
    void setShrineRace(Race_t race) {
        m_ShrineRace = race;
    }

    // get/set text color
    ZoneCoord_t getX() const {
        return m_X;
    }
    void setX(ZoneCoord_t x) {
        m_X = x;
    }

    // get/set text color
    ZoneCoord_t getY() const {
        return m_Y;
    }
    void setY(ZoneCoord_t y) {
        m_Y = y;
    }

private:
    ItemType_t m_ItemType; 

    ZoneID_t m_ZoneID;

    Storage_t m_Storage;
    string m_OwnerName;
    Race_t m_Race;
    Race_t m_ShrineRace;
    ZoneCoord_t m_X;
    ZoneCoord_t m_Y;
};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodBibleStatusFactory;
//
// Factory for GCBloodBibleStatus
//
//////////////////////////////////////////////////////////////////////

class GCBloodBibleStatusFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCBloodBibleStatus();
    }

    // get packet name
    string getPacketName() const {
        return "GCBloodBibleStatus";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_BLOOD_BIBLE_STATUS;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szItemType + szZoneID + szStorage + szBYTE + 255 + szRace + szRace + szZoneCoord + szZoneCoord;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodBibleStatusHandler;
//
//////////////////////////////////////////////////////////////////////

class GCBloodBibleStatusHandler {
public:
    // execute packet's handler
    static void execute(GCBloodBibleStatus* pPacket, Player* pPlayer);
};

#endif
