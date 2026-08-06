//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAttackArmsOK5.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ATTACK_ARMS_OK_5_H__
#define __GC_ATTACK_ARMS_OK_5_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK5;
//

//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK5 : public Packet {
public:
    // constructor
    GCAttackArmsOK5();

    // destructor
    ~GCAttackArmsOK5();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_ATTACK_ARMS_OK_5;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szSkillType + szObjectID + szObjectID + szbool;
    }

    // get packet's name
    string getPacketName() const {
        return "GCAttackArmsOK5";
    }

    // get packet's debug string
    string toString() const;

    SkillType_t getSkillType() {
        return m_SkillType;
    }
    void setSkillType(SkillType_t skillType) {
        m_SkillType = skillType;
    }

    // get / set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    // get / set ObjectID
    ObjectID_t getTargetObjectID() const {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t TargetObjectID) {
        m_TargetObjectID = TargetObjectID;
    }
    //	Coord_t getTargetX() const  { return m_X;}
    //	Coord_t getTargetY() const  { return m_Y;}
    //	void setTargetXY(Coord_t X, Coord_t Y)  { m_X = X; m_Y = Y;}

    // get / set success
    bool getSkillSuccess() const {
        return m_bSuccess;
    }
    void setSkillSuccess(bool bSuccess) {
        m_bSuccess = bSuccess;
    }

private:
    SkillType_t m_SkillType;
    // ObjectID
    ObjectID_t m_ObjectID;
    //	Coord_t m_X, m_Y;

    // TargetObjectID
    ObjectID_t m_TargetObjectID;

    // bool
    bool m_bSuccess;
};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK5Factory;
//
// Factory for GCAttackArmsOK5
//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK5Factory : public PacketFactory {
public:
    // constructor
    GCAttackArmsOK5Factory() {}

    // destructor
    virtual ~GCAttackArmsOK5Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCAttackArmsOK5();
    }

    // get packet name
    string getPacketName() const {
        return "GCAttackArmsOK5";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ATTACK_ARMS_OK_5;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szSkillType + szObjectID + szObjectID + szbool;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK5Handler;
//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK5Handler {
public:
    // execute packet's handler
    static void execute(GCAttackArmsOK5* pGCAttackArmsOK5, Player* pPlayer);
};

#endif
