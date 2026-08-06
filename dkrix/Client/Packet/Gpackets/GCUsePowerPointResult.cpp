//-------------------------------------------------------------------------------- // 
// Filename    : GCUsePowerPointResult.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCUsePowerPointResult.h"


//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
GCUsePowerPointResult::GCUsePowerPointResult ()
	throw ()
	: m_ErrorCode(0), m_ItemCode(0)
{
}

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
GCUsePowerPointResult::~GCUsePowerPointResult ()
	throw ()
{
}

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
void GCUsePowerPointResult::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// Error code
	iStream.read( m_ErrorCode );

	// Item Code
	iStream.read( m_ItemCode );

	// Power Point
	iStream.read( m_PowerPoint );

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCUsePowerPointResult::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// Error code
	oStream.write( m_ErrorCode );

	// Item Code
	oStream.write( m_ItemCode );

	// Power Point
	oStream.write( m_PowerPoint );

	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCUsePowerPointResult::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCUsePowerPointResultHandler::execute( this , pPlayer );

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCUsePowerPointResult::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "GCUsePowerPointResult("
		<< "ErrorCode:" << (int)m_ErrorCode
		<< ",ItemCode:" << (int)m_ItemCode
		<< ",PowerPoint:" << (int)m_PowerPoint
		<< ")";

	return msg.toString();

	__END_CATCH
}

#endif