//--------------------------------------------------------------------------------
// 
// Filename    : GCSearchMotorcycleFail.cpp 


// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCSearchMotorcycleFail.h"
#include "Packet/Assert.h"

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCSearchMotorcycleFail::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCSearchMotorcycleFail::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCSearchMotorcycleFail::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCSearchMotorcycleFailHandler::execute( this , pPlayer );

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCSearchMotorcycleFail::toString () const
		   throw ()
	{
		__BEGIN_TRY
			
		StringStream msg;
		msg << "GCSearchMotorcycleFail()";
		return msg.toString();
			
		__END_CATCH
	}
#endif

