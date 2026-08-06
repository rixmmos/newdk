//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSkillToObjectOK6.h 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SKILL_TO_OBJECT_OK_6_H__
#define __GC_SKILL_TO_OBJECT_OK_6_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../ModifyInfo.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK6;
//

//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK6 : public ModifyInfo {

public :
	
	// constructor
	GCSkillToObjectOK6() throw();
	
	// destructor
	~GCSkillToObjectOK6() throw();

	
public :
	
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SKILL_TO_OBJECT_OK_6; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize() const throw() { return szCoord*2 + szSkillType + szDuration + szBYTE + ModifyInfo::getPacketSize(); }


#ifdef __DEBUG_OUTPUT__
	// get packet's name
	std::string getPacketName() const throw() { return "GCSkillToObjectOK6"; }
	
	// get packet's debug string
	std::string toString() const throw();
#endif

	// get / set ObjectID
	Coord_t getX() const throw() { return m_X;}
	Coord_t getY() const throw() { return m_Y;}
	void setXY(Coord_t X, Coord_t Y) throw() { m_X = X; m_Y = Y;}
		

	// get / set SkillType
	SkillType_t getSkillType() const throw() { return m_SkillType; }
	void setSkillType(SkillType_t SkillType) throw() { m_SkillType = SkillType; }

	// get / set Duration
	Duration_t getDuration() const throw() { return m_Duration; }
	void setDuration(Duration_t Duration) throw() { m_Duration = Duration; }
	
	// get / set CEffectID 
//	CEffectID_t getCEffectID() const throw() { return m_CEffectID; }
//	void setCEffectID(CEffectID_t e) throw() { m_CEffectID = e; }

	BYTE getGrade() const throw() { return m_Grade; } 
	void setGrade( BYTE grade ) throw() { m_Grade = grade; }

private :

	// origin X, Y	
	Coord_t m_X, m_Y;

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
// class GCSkillToObjectOK6Factory;
//
// Factory for GCSkillToObjectOK6
//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK6Factory : public PacketFactory {

public :
	
	// constructor
	GCSkillToObjectOK6Factory() throw() {}
	
	// destructor
	virtual ~GCSkillToObjectOK6Factory() throw() {}

	
public :
	
	// create packet
	Packet* createPacket() throw() { return new GCSkillToObjectOK6(); }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCSkillToObjectOK6"; }
#endif
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SKILL_TO_OBJECT_OK_6; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szCoord*2+ szSkillType + szDuration + szBYTE + ModifyInfo::getPacketMaxSize(); }

};


//////////////////////////////////////////////////////////////////////
//
// class GCSkillToObjectOK6Handler;
//
//////////////////////////////////////////////////////////////////////

class GCSkillToObjectOK6Handler {

public :

	// execute packet's handler
	static void execute(GCSkillToObjectOK6* pGCSkillToObjectOK6, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
