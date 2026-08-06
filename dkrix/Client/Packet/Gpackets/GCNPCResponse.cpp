//////////////////////////////////////////////////////////////////////////////
// Filename    : GCNPCResponse.cpp
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCNPCResponse.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCNPCResponse::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read(m_Code);

	switch (m_Code)
	{
		
		case NPC_RESPONSE_REPAIR_OK:
		case NPC_RESPONSE_SILVER_COATING_OK:
		case NPC_RESPONSE_DONATION_OK:
		case NPC_RESPONSE_DECREASE_BALL:
		case NPC_RESPONSE_GUILD_SHOW_REGIST:
		case NPC_RESPONSE_SHOW_TAX_BALANCE:
		case NPC_RESPONSE_WITHDRAW_TAX_OK:				
		case NPC_RESPONSE_COUPLE_CANNOT_MEET:
		case NPC_RESPONSE_NOT_COUPLE:
		case NPC_RESPONSE_QUEST:
		case NPC_RESPONSE_LOTTERY :
		case NPC_RESPONSE_SHOW_TAX_RATIO :
		case NPC_RESPONSE_SHOW_DONATION_DAILOG:
		case NPC_RESPONSE_SHOW_CONFIRM_GET_EVENT_ITEM_DIALOG:
		case NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG:
			iStream.read(m_Parameter);
			break;
		
		default:
			break;
	}

	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCNPCResponse::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write(m_Code);

	switch (m_Code)
	{
		
		case NPC_RESPONSE_REPAIR_OK:
		case NPC_RESPONSE_SILVER_COATING_OK:
		case NPC_RESPONSE_DONATION_OK:
		case NPC_RESPONSE_DECREASE_BALL:
		case NPC_RESPONSE_GUILD_SHOW_REGIST:
		case NPC_RESPONSE_SHOW_TAX_BALANCE:
		case NPC_RESPONSE_WITHDRAW_TAX_OK:				
		case NPC_RESPONSE_COUPLE_CANNOT_MEET:
		case NPC_RESPONSE_NOT_COUPLE:
		case NPC_RESPONSE_QUEST:
		case NPC_RESPONSE_LOTTERY :
		case NPC_RESPONSE_SHOW_TAX_RATIO :
		case NPC_RESPONSE_SHOW_DONATION_DAILOG:
		case NPC_RESPONSE_SHOW_CONFIRM_GET_EVENT_ITEM_DIALOG:
		case NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG:
			oStream.write(m_Parameter);
			break;
		
		default:
			break;
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCNPCResponse::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCNPCResponseHandler::execute( this , pPlayer );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

PacketSize_t GCNPCResponse::getPacketSize () const 
	throw() 
{
	__BEGIN_TRY

	PacketSize_t size = szWORD;

	switch (m_Code)
	{
		
		case NPC_RESPONSE_REPAIR_OK:
		case NPC_RESPONSE_SILVER_COATING_OK:
		case NPC_RESPONSE_DONATION_OK:
		case NPC_RESPONSE_DECREASE_BALL:
		case NPC_RESPONSE_GUILD_SHOW_REGIST:
		case NPC_RESPONSE_SHOW_TAX_BALANCE:
		case NPC_RESPONSE_WITHDRAW_TAX_OK:				
		case NPC_RESPONSE_COUPLE_CANNOT_MEET:
		case NPC_RESPONSE_NOT_COUPLE:
		case NPC_RESPONSE_QUEST:
		case NPC_RESPONSE_LOTTERY :
        case NPC_RESPONSE_SHOW_TAX_RATIO :
		case NPC_RESPONSE_SHOW_DONATION_DAILOG:
		case NPC_RESPONSE_SHOW_CONFIRM_GET_EVENT_ITEM_DIALOG:
		case NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG:
			size += szuint;
			break;
		
		default:
			break;
	}

	return size;

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string GCNPCResponse::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCNPCResponse(" 
	    << "Code : " << (int)m_Code 
		<< "Parameter : " << (int)m_Parameter
	    << ")";
	return msg.toString();

	__END_CATCH
}
#endif