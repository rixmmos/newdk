//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSkillToSelfOK2.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SKILL_TO_SELF_OK_2_H__
#define __GC_SKILL_TO_SELF_OK_2_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCSkillToSelfOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCSkillToSelfOK2 : public Packet {

public :
	
	// constructor
	GCSkillToSelfOK2() throw();
	
	// destructor
	~GCSkillToSelfOK2() throw();

	
public :
	
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SKILL_TO_SELF_OK_2; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize() const throw() { return szObjectID + szSkillType + szDuration + szBYTE; }


#ifdef __DEBUG_OUTPUT__
	// get packet's name
	std::string getPacketName() const throw() { return "GCSkillToSelfOK2"; }
	
	// get packet's debug string
	std::string toString() const throw();
#endif

	// get / set ObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }

	// get / set SkillType
	SkillType_t getSkillType() const throw() { return m_SkillType; }
	void setSkillType(SkillType_t SkillType) throw() { m_SkillType = SkillType; }

	// get / set Duration
	Duration_t getDuration() const throw() { return m_Duration; }
	void setDuration(Duration_t Duration) throw() { m_Duration = Duration; }

	BYTE getGrade() const throw() { return m_Grade; }
	void setGrade( BYTE grade ) throw() { m_Grade = grade; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	// SkillType
	SkillType_t m_SkillType;

	// Duration
	Duration_t m_Duration;

	BYTE m_Grade;

};


//////////////////////////////////////////////////////////////////////
//
// class GCSkillToSelfOK2Factory;
//
// Factory for GCSkillToSelfOK2
//
//////////////////////////////////////////////////////////////////////

class GCSkillToSelfOK2Factory : public PacketFactory {

public :
	
	// constructor
	GCSkillToSelfOK2Factory() throw() {}
	
	// destructor
	virtual ~GCSkillToSelfOK2Factory() throw() {}

	
public :
	
	// create packet
	Packet* createPacket() throw() { return new GCSkillToSelfOK2(); }


#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCSkillToSelfOK2"; }
#endif
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SKILL_TO_SELF_OK_2; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szSkillType + szDuration + szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCSkillToSelfOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCSkillToSelfOK2Handler {

public :

	// execute packet's handler
	static void execute(GCSkillToSelfOK2* pGCSkillToSelfOK2, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
