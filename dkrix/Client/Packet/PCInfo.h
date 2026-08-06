//----------------------------------------------------------------------
//
// Filename    : PCInfo.h
// Writen By   : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __PC_INFO_H__
#define __PC_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//----------------------------------------------------------------------
//
// class PCInfo;
//




//
//----------------------------------------------------------------------

class PCInfo {

public :

	// destructor
	virtual ~PCInfo () {}	

	
	virtual PCType getPCType () const throw () = 0;	

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------

	// read data from socket input stream
	virtual void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error ) = 0;

	// write data to socket output stream
	virtual void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error ) = 0;

	// get size of object
	virtual uint getSize () const throw () = 0;

	#ifdef __DEBUG_OUTPUT__
		// get debug std::string
		virtual std::string toString () const throw () = 0;
	#endif

};

#endif
