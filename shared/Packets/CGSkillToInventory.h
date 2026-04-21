//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToInventory.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SKILL_TO_INVENTORY_H__
#define __CG_SKILL_TO_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGSkillToInventory : public Packet {
public:
    CGSkillToInventory() throw();
    ~CGSkillToInventory() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_SKILL_TO_INVENTORY;
    }
    PacketSize_t getPacketSize() const throw() {
        return szSkillType + szObjectID + szCoordInven * 4;
    }
    string getPacketName() const throw() {
        return "CGSkillToInventory";
    }
    string toString() const throw();

    SkillType_t getSkillType() const throw() {
        return m_SkillType;
    }
    void setSkillType(SkillType_t skillType) throw() {
        m_SkillType = skillType;
    }

    ObjectID_t getObjectID() const throw() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

    ObjectID_t getInventoryItemObjectID() throw() {
        return m_InventoryItemObjectID;
    }
    void setInventoryItemObjectID(ObjectID_t inventoryItemObjectID) throw() {
        m_InventoryItemObjectID = inventoryItemObjectID;
    }

    CoordInven_t getX() const throw() {
        return m_X;
    }
    void setX(Coord_t x) throw() {
        m_X = x;
    }

    CoordInven_t getY() const throw() {
        return m_Y;
    }
    void setY(Coord_t y) throw() {
        m_Y = y;
    }

    CoordInven_t getTargetX() const throw() {
        return m_TargetX;
    }
    void setTargetX(Coord_t targetX) throw() {
        m_TargetX = targetX;
    }

    CoordInven_t getTargetY() const throw() {
        return m_TargetY;
    }
    void setTargetY(Coord_t targetY) throw() {
        m_TargetY = targetY;
    }

private:
    SkillType_t m_SkillType;
    ObjectID_t m_ObjectID;
    ObjectID_t m_InventoryItemObjectID;
    CoordInven_t m_X;
    CoordInven_t m_Y;
    CoordInven_t m_TargetX;
    CoordInven_t m_TargetY;
};

class CGSkillToInventoryFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGSkillToInventory();
    }
    string getPacketName() const throw() {
        return "CGSkillToInventory";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SKILL_TO_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType + szObjectID + szCoordInven * 4;
    }
};

class CGSkillToInventoryHandler {
public:
    static void execute(CGSkillToInventory* pCGSkillToInventory, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
