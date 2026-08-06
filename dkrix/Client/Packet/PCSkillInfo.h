//////////////////////////////////////////////////////////////////////
// 
// Filename    : PCSkillInfo.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __PC_SKILL_INFO_H__
#define __PC_SKILL_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class PCSkillInfo;
//

//
//////////////////////////////////////////////////////////////////////

class PCSkillInfo {

public :
	
	// constructor
	PCSkillInfo () throw () {}
	
	// destructor
	virtual ~PCSkillInfo () throw () {}

public :

    
    virtual void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error ) = 0;
		    
    
    virtual void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error ) = 0;

	// get packet's body size
	
	virtual PacketSize_t getSize () throw () = 0;

	#ifdef __DEBUG_OUTPUT__
		// get packet's debug std::string
		virtual std::string toString () const throw () = 0;
	#endif

};

#endif
