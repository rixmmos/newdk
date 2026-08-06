//////////////////////////////////////////////////////////////////////////////
// Filename    : GCNoticeEvent.cpp
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCNoticeEvent.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCNoticeEvent::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read(m_Code);

	switch (m_Code)
	{
		
		case NOTICE_EVENT_MASTER_COMBAT_TIME :
		case NOTICE_EVENT_KICK_OUT_FROM_ZONE :
		case NOTICE_EVENT_CONTINUAL_GROUND_ATTACK :
		case NOTICE_EVENT_METEOR_STRIKE :
		case NOTICE_EVENT_SHOP_TAX_CHANGE :
		case NOTICE_EVENT_WAR_OVER :
		case NOTICE_EVENT_RESULT_LOTTERY :
		case NOTICE_EVENT_MASTER_LAIR_OPEN :
		case NOTICE_EVENT_MASTER_LAIR_CLOSED :
		case NOTICE_EVENT_MASTER_LAIR_COUNT :
		case NOTICE_EVENT_MINI_GAME:
		case NOTICE_EVENT_FLAG_WAR_FINISH:
		case NOTICE_EVENT_LEVEL_WAR_ARRANGED :            
		case NOTICE_EVENT_LEVEL_WAR_STARTED :             
		case NOTICE_EVENT_RACE_WAR_SOON:
		case NOTICE_EVENT_ENTER_BEGINNER_ZONE:
		case NOTICE_EVENT_FLAG_WAR_READY:
		case NOTICE_EVENT_LOGIN_JUST_NOW:
		case NOTICE_EVENT_HOLYDAY:
		case NOTICE_EVENT_GOLD_MEDALS:
		case NOTICE_EVENT_CROWN_PRICE:
			iStream.read(m_Parameter);
			break;
		
		default:
			break;
	}

	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCNoticeEvent::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write(m_Code);

	switch (m_Code)
	{
		
		case NOTICE_EVENT_MASTER_COMBAT_TIME :
		case NOTICE_EVENT_KICK_OUT_FROM_ZONE :
		case NOTICE_EVENT_CONTINUAL_GROUND_ATTACK :
		case NOTICE_EVENT_METEOR_STRIKE :
		case NOTICE_EVENT_SHOP_TAX_CHANGE :
		case NOTICE_EVENT_WAR_OVER :
		case NOTICE_EVENT_RESULT_LOTTERY :
		case NOTICE_EVENT_MASTER_LAIR_OPEN :
		case NOTICE_EVENT_MASTER_LAIR_CLOSED :
		case NOTICE_EVENT_MASTER_LAIR_COUNT :
		case NOTICE_EVENT_MINI_GAME:
		case NOTICE_EVENT_FLAG_WAR_FINISH:
		case NOTICE_EVENT_LEVEL_WAR_ARRANGED :            
		case NOTICE_EVENT_LEVEL_WAR_STARTED :             
		case NOTICE_EVENT_RACE_WAR_SOON:
		case NOTICE_EVENT_ENTER_BEGINNER_ZONE:
		case NOTICE_EVENT_FLAG_WAR_READY:
		case NOTICE_EVENT_LOGIN_JUST_NOW:
		case NOTICE_EVENT_HOLYDAY:
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
void GCNoticeEvent::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCNoticeEventHandler::execute( this , pPlayer );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

PacketSize_t GCNoticeEvent::getPacketSize () const 
	throw() 
{
	__BEGIN_TRY

	PacketSize_t size = szWORD;

	switch (m_Code)
	{
		
		case NOTICE_EVENT_MASTER_COMBAT_TIME :
		case NOTICE_EVENT_KICK_OUT_FROM_ZONE :
		case NOTICE_EVENT_CONTINUAL_GROUND_ATTACK :
		case NOTICE_EVENT_METEOR_STRIKE :
		case NOTICE_EVENT_SHOP_TAX_CHANGE :
		case NOTICE_EVENT_WAR_OVER :
		case NOTICE_EVENT_RESULT_LOTTERY :
		case NOTICE_EVENT_MASTER_LAIR_OPEN :
		case NOTICE_EVENT_MASTER_LAIR_CLOSED :
		case NOTICE_EVENT_MASTER_LAIR_COUNT :
		case NOTICE_EVENT_MINI_GAME:
		case NOTICE_EVENT_FLAG_WAR_FINISH:
		case NOTICE_EVENT_LEVEL_WAR_ARRANGED :            
		case NOTICE_EVENT_LEVEL_WAR_STARTED :             
		case NOTICE_EVENT_RACE_WAR_SOON:
		case NOTICE_EVENT_ENTER_BEGINNER_ZONE:
		case NOTICE_EVENT_FLAG_WAR_READY:
		case NOTICE_EVENT_LOGIN_JUST_NOW:
		case NOTICE_EVENT_HOLYDAY:
			//		case NOTICE_EVENT_RACE_WAR_TODAY:
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
std::string GCNoticeEvent::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCNoticeEvent(" 
	    << "Code : " << (int)m_Code 
		<< "Parameter : " << (int)m_Parameter
	    << ")";
	return msg.toString();

	__END_CATCH
}
#endif