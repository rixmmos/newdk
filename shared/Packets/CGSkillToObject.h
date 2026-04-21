//////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToObject.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SKILL_TO_OBJECT_H__
#define __CG_SKILL_TO_OBJECT_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGSkillToObject : public Packet {
public:
    CGSkillToObject() throw();
    ~CGSkillToObject() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_SKILL_TO_OBJECT;
    }
    PacketSize_t getPacketSize() const throw() {
        return szSkillType + szCEffectID + szObjectID;
    }
    string getPacketName() const throw() {
        return "CGSkillToObject";
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

    ObjectID_t getTargetObjectID() const throw() {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t targetObjectID) throw() {
        m_TargetObjectID = targetObjectID;
    }

private:
    SkillType_t m_SkillType;
    CEffectID_t m_CEffectID;
    ObjectID_t m_TargetObjectID;
};

class CGSkillToObjectFactory : public PacketFactory {
public:
    CGSkillToObjectFactory() throw() {}
    virtual ~CGSkillToObjectFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGSkillToObject();
    }
    string getPacketName() const throw() {
        return "CGSkillToObject";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SKILL_TO_OBJECT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType + szCEffectID + szObjectID;
    }
};

class CGSkillToObjectHandler {
public:
    static void execute(CGSkillToObject* pCGSkillToObject, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
