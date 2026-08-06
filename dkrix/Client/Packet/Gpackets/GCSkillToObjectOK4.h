//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSkillToObjectOK4.h 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SKILL_TO_OBJECT_OK_4_H__
#define __GC_SKILL_TO_OBJECT_OK_4_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK4;
//

//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK4 : public Packet {

public :
	
	// constructor
	GCSkillToObjectOK4() throw();
	
	// destructor
	~GCSkillToObjectOK4() throw();

	
public :
	
	

    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SKILL_TO_OBJECT_OK_4; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize() const throw() { return szObjectID + szSkillType + szDuration + szBYTE; }

#ifdef __DEBUG_OUTPUT__
	// get packet's name
	std::string getPacketName() const throw() { return "GCSkillToObjectOK4"; }
	
	// get packet's debug string
	std::string toString() const throw();
#endif

	// get / set ObjectID
	ObjectID_t getTargetObjectID() const throw() { return m_ObjectID; }
	void setTargetObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }


	// get / set SkillType
	SkillType_t getSkillType() const throw() { return m_SkillType; }
	void setSkillType(SkillType_t SkillType) throw() { m_SkillType = SkillType; }
	
/*
	// get / set Target X,Y
	Coord_t getTargetX() const throw() { return m_TargetX; }
	Coord_t getTargetY() const throw() { return m_TargetY; }
	void setTargetXY(Coord_t X, Coord_t Y) throw() { m_TargetX = X; m_TargetY = Y; }
*/
	
	// get / set Duration
	Duration_t getDuration() const throw() { return m_Duration; }
	void setDuration(Duration_t Duration) throw() { m_Duration = Duration; }
	
	// get / set CEffectID 
//	CEffectID_t getCEffectID() const throw() { return m_CEffectID; }
//	void setCEffectID(CEffectID_t e) throw() { m_CEffectID = e; }

	BYTE getGrade() const throw() { return m_Grade; }
	void setGrade( BYTE grade ) throw() { m_Grade = grade; }

private :
	
	// ObjectID(Target)
	ObjectID_t m_ObjectID;

	// SkillType
	SkillType_t m_SkillType;

	// Duration
	Duration_t m_Duration;

	// CEffectID
//	CEffectID_t m_CEffectID;

	BYTE m_Grade;

};


//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK4Factory;
//
// Factory for GCSkillToObjectOK4
//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK4Factory : public PacketFactory {

public :
	
	// constructor
	GCSkillToObjectOK4Factory() throw() {}
	
	// destructor
	virtual ~GCSkillToObjectOK4Factory() throw() {}

	
public :
	
	// create packet
	Packet* createPacket() throw() { return new GCSkillToObjectOK4(); }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCSkillToObjectOK4"; }
#endif
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SKILL_TO_OBJECT_OK_4; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szSkillType + szDuration + szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK4Handler;
//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK4Handler {

public :

	// execute packet's handler
	static void execute(GCSkillToObjectOK4* pGCSkillToObjectOK4, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
