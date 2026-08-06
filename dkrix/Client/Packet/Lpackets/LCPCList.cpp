//----------------------------------------------------------------------
// 
// Filename    : LCPCList.cpp 
// Written By  : Reiot
// Description :
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCPCList.h"
#include "../PCSlayerInfo.h"
#include "../PCVampireInfo.h"
#include "../PCOustersInfo.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
LCPCList::LCPCList ()
	throw ()
{
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ )
		m_pPCInfos[i] = NULL;
}


//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
LCPCList::~LCPCList ()
	throw ()
{
	
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ ) {
		if ( m_pPCInfos[i] != NULL ) {
			delete m_pPCInfos[i];
			m_pPCInfos[i] = NULL;
		}
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void LCPCList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	//--------------------------------------------------
	
	//
	// *OPTMIZATION*
	//
	
	//
	//--------------------------------------------------
	char pcTypes[SLOT_MAX];

	for ( uint i = 0 ; i < SLOT_MAX ; i ++ )
		iStream.read(pcTypes[i]);

	//--------------------------------------------------
	
	//--------------------------------------------------
	for ( uint j = 0 ; j < SLOT_MAX ; j ++ ) {

		switch ( pcTypes[j] ) {

			case 'S' :
				{
					PCSlayerInfo * pPCSlayerInfo = new PCSlayerInfo();	
					pPCSlayerInfo->read( iStream );
					m_pPCInfos[ pPCSlayerInfo->getSlot() ] = pPCSlayerInfo;
				}
				break;

			case 'V' :
				{
					PCVampireInfo * pPCVampireInfo = new PCVampireInfo();	
					pPCVampireInfo->read( iStream );
					m_pPCInfos[ pPCVampireInfo->getSlot() ] = pPCVampireInfo;
				}
				break;

			case 'O' :
				{
					PCOustersInfo * pPCOustersInfo = new PCOustersInfo();	
					pPCOustersInfo->read( iStream );
					m_pPCInfos[ pPCOustersInfo->getSlot() ] = pPCOustersInfo;
				}
				break;
				
			case '0' :
				break;

			default :
				throw InvalidProtocolException("invalid pc type");
		}

	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LCPCList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	//--------------------------------------------------
	
	//
	
	//
	// ex>
	// 	S0V : Slayer-EMPTY-VAMPIRE
	// 	00S : EMPTY-EMPTY-SLAYER
	//
	//--------------------------------------------------
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ ) {

		if ( m_pPCInfos[i] ) {	// m_pPCInfos[i] != NULL

			switch(m_pPCInfos[i]->getPCType())
			{
			case PC_SLAYER:
				oStream.write( 'S' );
				break;

			case PC_VAMPIRE:
				oStream.write( 'V' );
				break;

			case PC_OUSTERS:
				oStream.write( 'O' );
				break;
			}
		} else {				// m_pPCInfos[i] == NULL
			oStream.write( '0' );
		}
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	for ( uint j = 0 ; j < SLOT_MAX ; j ++ ) {
		if ( m_pPCInfos[j] != NULL ) {
			m_pPCInfos[j]->write( oStream );
		}
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void LCPCList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	LCPCListHandler::execute( this , pPlayer );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
PacketSize_t LCPCList::getPacketSize () const 
	throw ()
{
	PacketSize_t packetSize = 0;

	packetSize = SLOT_MAX*sizeof(char);
	
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ ) {
		if ( m_pPCInfos[i] ) { // m_pPCInfos[i] != NULL
			packetSize += m_pPCInfos[i]->getSize();
		}
	}
	return packetSize;
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string LCPCList::toString () const
		   throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "LCPCList(\n";

		for ( uint i = 0 ; i < SLOT_MAX ; i ++ )
			if ( m_pPCInfos[i] != NULL )
				msg << m_pPCInfos[i]->toString() << "\n";
			else
				msg << "EMPTY SLOT\n";

		msg << ")";

		return msg.toString();

		__END_CATCH
	}


#endif
