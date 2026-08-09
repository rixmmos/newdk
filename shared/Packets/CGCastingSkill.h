//////////////////////////////////////////////////////////////////////
//
// Filename    : CGCastingSkill.h
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_CASTING_SKILL_H__
#define __CG_CASTING_SKILL_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGCastingSkill;
//
//////////////////////////////////////////////////////////////////////

class CGCastingSkill : public Packet {
public:
    // constructor
    CGCastingSkill();

    // destructor
    ~CGCastingSkill();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_CASTING_SKILL;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szSkillType;
    }

    // get packet name
    string getPacketName() const {
        return "CGCastingSkill";
    }

    // get/set SkillType
    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setObjectID(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

    // get packet's debug string
    string toString() const;

private:
    // SkillType
    SkillType_t m_SkillType;
};


//////////////////////////////////////////////////////////////////////
//
// class CGCastingSkillFactory;
//
// Factory for CGCastingSkill
//
//////////////////////////////////////////////////////////////////////

class CGCastingSkillFactory : public PacketFactory {
public:
    // constructor
    CGCastingSkillFactory() {}

    // destructor
    virtual ~CGCastingSkillFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGCastingSkill();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGCastingSkill";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_CASTING_SKILL;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGCastingSkillHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGCastingSkillHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGCastingSkillHandler {
public:
    // execute packet's handler
    static void execute(CGCastingSkill* pCGCastingSkill, Player* pPlayer);
};
#endif

#endif
