//----------------------------------------------------------------------
//
// Filename    : SubVampireSkillInfo.h
// Written By  : elca
// Description :
//
//----------------------------------------------------------------------

#ifndef __SUB_VAMPIRE_SKILL_INFO_H__
#define __SUB_VAMPIRE_SKILL_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//----------------------------------------------------------------------
//

//


//
//----------------------------------------------------------------------

class SubVampireSkillInfo {

public :

	// read data from socket input stream
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );

	// write data to socket output stream
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// get size of object
	uint getSize () const throw () { return szSkillType + szTurn + szTurn; }
	// get max size of object
	static uint getMaxSize () throw () { return szSkillType + szTurn + szTurn; }

	#ifdef __DEBUG_OUTPUT__
		// get debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get / set SkillType
	SkillType_t getSkillType() const throw() { return m_SkillType; }
	void setSkillType( SkillType_t SkillType ) throw() { m_SkillType = SkillType; }

	// get / set Turn
	Turn_t getSkillTurn() const throw() { return m_Interval ; }
	void setSkillTurn( Turn_t SkillTurn ) throw() { m_Interval = SkillTurn; }

	// get / set CastingTime
	Turn_t getCastingTime() const throw() { return m_CastingTime; }
	void setCastingTime( Turn_t CastingTime ) throw() { m_CastingTime = CastingTime; }

private :

	
	SkillType_t m_SkillType;

	
	Turn_t m_Interval;

	
	Turn_t m_CastingTime;

};

#endif
