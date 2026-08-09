//////////////////////////////////////////////////////////////////////////////
// Filename    : CGLearnSkill.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_LEARN_SKILL_H__
#define __CG_LEARN_SKILL_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGLearnSkill;
//////////////////////////////////////////////////////////////////////////////

class CGLearnSkill : public Packet {
public:
    CGLearnSkill() {};
    ~CGLearnSkill() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_LEARN_SKILL;
    }
    PacketSize_t getPacketSize() const {
        return szSkillType + szSkillDomainType;
    }
    string getPacketName() const {
        return "CGLearnSkill";
    }
    string toString() const;

public:
    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

    SkillDomainType_t getSkillDomainType() const {
        return m_DomainType;
    }
    void setSkillDomainType(SkillDomainType_t DomainType) {
        m_DomainType = DomainType;
    }

private:
    SkillType_t m_SkillType;        
    SkillDomainType_t m_DomainType; 
};

//////////////////////////////////////////////////////////////////////
// class CGLearnSkillFactory;
//////////////////////////////////////////////////////////////////////

class CGLearnSkillFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGLearnSkill();
    }
    string getPacketName() const throw() {
        return "CGLearnSkill";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_LEARN_SKILL;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType + szSkillDomainType;
    }
};


//////////////////////////////////////////////////////////////////////
// class CGLearnSkillHandler;
//////////////////////////////////////////////////////////////////////

// Server-only: CGLearnSkillHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGLearnSkillHandler {
public:
    static void execute(CGLearnSkill* pCGLearnSkill, Player* pPlayer);
    static void executeSlayerSkill(CGLearnSkill* pCGLearnSkill, Player* pPlayer);
    static void executeVampireSkill(CGLearnSkill* pCGLearnSkill, Player* pPlayer);
    static void executeOustersSkill(CGLearnSkill* pCGLearnSkill, Player* pPlayer);
};
#endif

#endif
