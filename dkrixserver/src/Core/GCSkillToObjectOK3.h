//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToObjectOK3.h
// Written By  : elca@ewestsoft.com


//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SKILL_TO_OBJECT_OK_3_H__
#define __GC_SKILL_TO_OBJECT_OK_3_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK3;
//

//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK3 : public Packet {
public:
    // constructor
    GCSkillToObjectOK3();

    // destructor
    ~GCSkillToObjectOK3();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_SKILL_TO_OBJECT_OK_3;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szSkillType + szCoord * 2 + szBYTE;
    }

    // get packet's name
    string getPacketName() const {
        return "GCSkillToObjectOK3";
    }

    // get packet's debug string
    string toString() const;

    // get / set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    // get / set SkillType
    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

    // get / set Target X,Y
    Coord_t getTargetX() const {
        return m_TargetX;
    }
    Coord_t getTargetY() const {
        return m_TargetY;
    }
    void setTargetXY(Coord_t X, Coord_t Y) {
        m_TargetX = X;
        m_TargetY = Y;
    }


    // get / set CEffectID
    //	CEffectID_t getCEffectID() const  { return m_CEffectID; }
    //	void setCEffectID(CEffectID_t e)  { m_CEffectID = e; }

    BYTE getGrade() const {
        return m_Grade;
    }
    void setGrade(BYTE grade) {
        m_Grade = grade;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;

    // SkillType
    SkillType_t m_SkillType;

    // TargetObjectID
    Coord_t m_TargetX, m_TargetY;


    // CEffectID
    //	CEffectID_t m_CEffectID;

    BYTE m_Grade;
};


//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK3Factory;
//
// Factory for GCSkillToObjectOK3
//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK3Factory : public PacketFactory {
public:
    // constructor
    GCSkillToObjectOK3Factory() {}

    // destructor
    virtual ~GCSkillToObjectOK3Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCSkillToObjectOK3();
    }

    // get packet name
    string getPacketName() const {
        return "GCSkillToObjectOK3";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_SKILL_TO_OBJECT_OK_3;
    }

    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szSkillType + szCoord * 2 + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK3Handler;
//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK3Handler {
public:
    // execute packet's handler
    static void execute(GCSkillToObjectOK3* pGCSkillToObjectOK3, Player* pPlayer);
};

#endif
