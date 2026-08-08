//////////////////////////////////////////////////////////////////////////////
// Filename    : CGDownSkill.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_DOWN_SKILL_H__
#define __CG_DOWN_SKILL_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGDownSkill;
//////////////////////////////////////////////////////////////////////////////

class CGDownSkill : public Packet {
public:
    CGDownSkill() {};
    ~CGDownSkill() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_DOWN_SKILL;
    }
    PacketSize_t getPacketSize() const {
        return szSkillType;
    }
    string getPacketName() const {
        return "CGDownSkill";
    }
    string toString() const;

public:
    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

private:
    SkillType_t m_SkillType;
};

//////////////////////////////////////////////////////////////////////
// class CGDownSkillFactory;
//////////////////////////////////////////////////////////////////////

class CGDownSkillFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGDownSkill();
    }
    string getPacketName() const throw() {
        return "CGDownSkill";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DOWN_SKILL;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType;
    }
};


//////////////////////////////////////////////////////////////////////
// class CGDownSkillHandler;
//////////////////////////////////////////////////////////////////////

// Server-only: CGDownSkillHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGDownSkillHandler {
public:
    static void execute(CGDownSkill* pCGDownSkill, Player* pPlayer);
};
#endif

#endif
