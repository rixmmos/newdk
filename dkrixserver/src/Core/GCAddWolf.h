//--------------------------------------------------------------------------------
//
// Filename    : GCAddWolf.h
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __GC_ADD_WOLF_H__
#define __GC_ADD_WOLF_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCAddWolf;
//

//
//--------------------------------------------------------------------------------
class GCAddWolf : public Packet {
public:
    GCAddWolf() {};
    ~GCAddWolf() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_ADD_WOLF;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE +
               m_Name.size()
               //			+ szSpriteType
               //			+ szColor
               + szColor + szItemType + szCoord + szCoord + szDir + szHP * 2 + szGuildID;
    }

    // get packet's name
    string getPacketName() const {
        return "GCAddWolf";
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

    /*
        // get/set sprite type
        SpriteType_t getSpriteType() const  { return m_SpriteType; }
        void setSpriteType(SpriteType_t spriteType)  { m_SpriteType = spriteType; }

        // get/set sub color
        Color_t getSubColor() const  { return m_SubColor; }
        void setSubColor(Color_t color)  { m_SubColor = color; }
    */
    // get/set main color
    Color_t getMainColor() const {
        return m_MainColor;
    }
    void setMainColor(Color_t color) {
        m_MainColor = color;
    }

    // get/set X
    Coord_t getX() const {
        return m_X;
    }
    void setXYDir(Coord_t x, Coord_t y, Dir_t Dir) {
        m_X = x;
        m_Y = y;
        m_Dir = Dir;
    }

    // get/set Y
    Coord_t getY() const {
        return m_Y;
    }

    // get/set Dir
    Dir_t getDir() const {
        return m_Dir;
    }

    // get /set MaxHP
    HP_t getMaxHP() const {
        return m_MaxHP;
    }
    void setMaxHP(HP_t MaxHP) {
        m_MaxHP = MaxHP;
    }

    // get /set CurrentHP
    HP_t getCurrentHP() const {
        return m_CurrentHP;
    }
    void setCurrentHP(HP_t CurrentHP) {
        m_CurrentHP = CurrentHP;
    }

    // get/set ItemType
    ItemType_t getItemType() const {
        return m_ItemType;
    }
    void setItemType(ItemType_t ItemType) {
        m_ItemType = ItemType;
    }

    // get/set GuildID
    GuildID_t getGuildID() const {
        return m_GuildID;
    }
    void setGuildID(GuildID_t GuildID) {
        m_GuildID = GuildID;
    }

private:
    
    ObjectID_t m_ObjectID;

    
    string m_Name;

    
    ItemType_t m_ItemType;

     
    Color_t m_MainColor;

    
    Coord_t m_X;
    Coord_t m_Y;
    Dir_t m_Dir;

    HP_t m_CurrentHP;
    HP_t m_MaxHP;
    GuildID_t m_GuildID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCAddWolfFactory;
//
// Factory for GCAddWolf
//
//////////////////////////////////////////////////////////////////////

class GCAddWolfFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCAddWolf();
    }

    // get packet name
    string getPacketName() const {
        return "GCAddWolf";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ADD_WOLF;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szBYTE +
               20
               //			+ szSpriteType
               //			+ szColor
               + szColor + szItemType + szCoord + szCoord + szDir + szHP * 2 + szGuildID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCAddWolfHandler;
//
//////////////////////////////////////////////////////////////////////

class GCAddWolfHandler {
public:
    // execute packet's handler
    static void execute(GCAddWolf* pPacket, Player* pPlayer);
};

#endif
