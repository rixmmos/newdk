//--------------------------------------------------------------------------------
// 
// Filename   : Assert.cpp 
// Written By : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "Packet/Types.h"
#include "Assert.h"
#include "Exception.h"
#include <time.h>

//--------------------------------------------------------------------------------
//
// __assert__
//

//
//--------------------------------------------------------------------------------
void __assert__ ( const char * file , uint line , const char * func , const char * expr )
	throw ( AssertionError )
{
	StringStream msg;
	
	msg << eos
		<< "Assertion Failed : " << file << " : " << line;

	if ( func )
		msg << " : " << func;

	time_t currentTime = time(0);
	
	msg << expr << " at " << ctime(&currentTime);
	
	ofstream ofile("assertion_failed.log",ios::app);
	ofile << msg.toString().c_str() << endl;
	ofile.close();

	throw AssertionError( msg.toString() );
}
