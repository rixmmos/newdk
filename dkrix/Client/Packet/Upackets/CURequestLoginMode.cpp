//--------------------------------------------------------------------------------
// 
// Filename    : CURequestLoginMode.cpp 
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "CURequestLoginMode.h"


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CURequestLoginMode::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CURequestLoginMode::read ( Socket * pSocket ) 
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__END_CATCH
}

	    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CURequestLoginMode::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void CURequestLoginMode::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	CURequestLoginModeHandler::execute ( this , pPlayer );
		
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string CURequestLoginMode::toString () const
	throw ()
{

	return "CUReqeustLoginMode()";
}
