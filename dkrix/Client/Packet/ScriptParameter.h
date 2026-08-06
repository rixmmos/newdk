//////////////////////////////////////////////////////////////////////
// 
// Filename    : ScriptParameter.h 
// Written By  : 
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __SCRIPT_PARAMETER_H__
#define __SCRIPT_PARAMETER_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class ScriptParameter;
//
// 
//
//////////////////////////////////////////////////////////////////////

class ScriptParameter {

public :
	
	// constructor
	ScriptParameter () throw ();
	
	// destructor
	~ScriptParameter () throw ();

public :
	
	
    
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
		    
    
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	// get packet's body size
	
	PacketSize_t getSize () throw ();

	static uint getMaxSize() throw() {
		return szBYTE + 255 + szBYTE + 255;
	}

	// get packet's debug std::string
	std::string toString () const throw ();

	// get/set Name
	std::string getName() const throw() { return m_Name; }
	void setName( const std::string& name ) throw() { m_Name = name; }

	// get/set Value
	std::string getValue() const throw() { return m_Value; }
	void setValue( const std::string& value ) throw() { m_Value = value; }

private :

	std::string m_Name;
	std::string m_Value;
};

#endif
