//////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToTile.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SKILL_TO_TILE_H__
#define __CG_SKILL_TO_TILE_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

class CGSkillToTile : public Packet {
public:
    CGSkillToTile() throw();
    ~CGSkillToTile() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_SKILL_TO_TILE;
    }
    PacketSize_t getPacketSize() const throw() {
        return szSkillType + szCEffectID + szCoord + szCoord;
    }
    string getPacketName() const throw() {
        return "CGSkillToTile";
    }
    string toString() const throw();

    SkillType_t getSkillType() const throw() {
        return m_SkillType;
    }
    void setSkillType(SkillType_t skillType) throw() {
        m_SkillType = skillType;
    }

    CEffectID_t getCEffectID() const throw() {
        return m_CEffectID;
    }
    void setCEffectID(CEffectID_t cEffectID) throw() {
        m_CEffectID = cEffectID;
    }

    Coord_t getX() const throw() {
        return m_X;
    }
    void setX(Coord_t x) throw() {
        m_X = x;
    }

    Coord_t getY() const throw() {
        return m_Y;
    }
    void setY(Coord_t y) throw() {
        m_Y = y;
    }

private:
    SkillType_t m_SkillType;
    CEffectID_t m_CEffectID;
    Coord_t m_X;
    Coord_t m_Y;
};

class CGSkillToTileFactory : public PacketFactory {
public:
    CGSkillToTileFactory() throw() {}
    virtual ~CGSkillToTileFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGSkillToTile();
    }
    string getPacketName() const throw() {
        return "CGSkillToTile";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SKILL_TO_TILE;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType + szCEffectID + szCoord + szCoord;
    }
};

class CGSkillToTileHandler {
public:
    static void execute(CGSkillToTile* pCGSkillToTile, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
