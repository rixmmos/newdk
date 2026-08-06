//--------------------------------------------------------------------------------
//
// Filename    : GCAddNPC.h
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __GC_ADD_NPC_H__
#define __GC_ADD_NPC_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCAddNPC;
//

//
//--------------------------------------------------------------------------------

class GCAddNPC : public Packet {
public:
    GCAddNPC() {};
    ~GCAddNPC() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_ADD_NPC;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE + m_Name.size() + szNPCID + szSpriteType + szColor + szColor + szCoord + szCoord +
               szDir;
    }

    // get packet's name
    string getPacketName() const {
        return "GCAddNPC";
    }

    // get packet's debug string
    string toString() const;


public:
    // get/set object id
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t creatureID) {
        m_ObjectID = creatureID;
    }

    // get/set name
    string getName() const {
        return m_Name;
    }
    void setName(string name) {
        m_Name = name;
    }

    // get/set NPC id
    NPCID_t getNPCID(void) const {
        return m_NPCID;
    }
    void setNPCID(NPCID_t NPCID) {
        m_NPCID = NPCID;
    }

    // get/set sprite type
    SpriteType_t getSpriteType() const {
        return m_SpriteType;
    }
    void setSpriteType(SpriteType_t spriteType) {
        m_SpriteType = spriteType;
    }

    // get/set main color
    Color_t getMainColor() const {
        return m_MainColor;
    }
    void setMainColor(Color_t color) {
        m_MainColor = color;
    }

    // get/set sub color
    Color_t getSubColor() const {
        return m_SubColor;
    }
    void setSubColor(Color_t color) {
        m_SubColor = color;
    }

    // get/set X
    Coord_t getX() const {
        return m_X;
    }
    void setX(Coord_t x) {
        m_X = x;
    }

    // get/set Y
    Coord_t getY() const {
        return m_Y;
    }
    void setY(Coord_t y) {
        m_Y = y;
    }

    // get/set Dir
    Dir_t getDir() const {
        return m_Dir;
    }
    void setDir(Dir_t dir) {
        m_Dir = dir;
    }

private:
    
    ObjectID_t m_ObjectID;

    
    string m_Name;

    
    NPCID_t m_NPCID;

    
    SpriteType_t m_SpriteType;

    
    Color_t m_MainColor;
    Color_t m_SubColor;

    
    Coord_t m_X;
    Coord_t m_Y;
    Dir_t m_Dir;
};


//////////////////////////////////////////////////////////////////////
//
// class GCAddNPCFactory;
//
// Factory for GCAddNPC
//
//////////////////////////////////////////////////////////////////////

class GCAddNPCFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCAddNPC();
    }

    // get packet name
    string getPacketName() const {
        return "GCAddNPC";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ADD_NPC;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szBYTE + 40 + szNPCID + szSpriteType + szColor + szColor + szCoord + szCoord + szDir;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCAddNPCHandler;
//
//////////////////////////////////////////////////////////////////////

class GCAddNPCHandler {
public:
    // execute packet's handler
    static void execute(GCAddNPC* pPacket, Player* pPlayer);
};

#endif
