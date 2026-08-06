//----------------------------------------------------------------------
//
// Filename    : PacketIDSet.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#include "Client_PCH.h"
// include files
#include "PacketIDSet.h"
#include "Assert.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
PacketIDSet::PacketIDSet ( PlayerStatus playerStatus , PacketIDSetType packetIDSetType ) 
	throw ()
: m_PacketIDSetType(packetIDSetType), m_PlayerStatus(playerStatus)
{
}

//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
PacketIDSet::~PacketIDSet () 
	throw ()
{
	m_PacketIDSet.clear();
}

//----------------------------------------------------------------------
// add packet id to set
//----------------------------------------------------------------------
void PacketIDSet::addPacketID ( PacketID_t packetID ) 
	throw ( DuplicatedException )
{
	__BEGIN_TRY

	Assert( m_PacketIDSetType != PIST_NONE );
	Assert( m_PacketIDSetType != PIST_ANY );

	std::pair<PACKET_ID_SET::iterator,bool> p = m_PacketIDSet.insert( packetID );

	
	if ( ! p.second )
		throw DuplicatedException();

	__END_CATCH
}

//----------------------------------------------------------------------
// delete packet id from set
//----------------------------------------------------------------------
void PacketIDSet::deletePacketID ( PacketID_t packetID ) 
	throw ( NoSuchElementException )
{
	__BEGIN_TRY

	PACKET_ID_SET::iterator itr = m_PacketIDSet.find( packetID );

	if ( itr != m_PacketIDSet.end() )
		throw NoSuchElementException();

	m_PacketIDSet.erase(itr);

	__END_CATCH
}

//----------------------------------------------------------------------
// has packet id ?
//----------------------------------------------------------------------
bool PacketIDSet::hasPacketID ( PacketID_t packetID ) const
	throw ( NoSuchElementException , IgnorePacketException )
{
	__BEGIN_TRY

	if ( m_PacketIDSetType == PIST_NORMAL ) {

		
		PACKET_ID_SET::const_iterator itr = m_PacketIDSet.find( packetID );

		return itr != m_PacketIDSet.end();

	} else if ( m_PacketIDSetType == PIST_ANY ) {

		
		return true;

	} else if ( m_PacketIDSetType == PIST_IGNORE_EXCEPT ) {

		
		
		PACKET_ID_SET::const_iterator itr = m_PacketIDSet.find( packetID );

		if ( itr != m_PacketIDSet.end() ) {
			return true;
		} else {
			throw IgnorePacketException();
		}
	}

	// case of PIST_NONE
	return false;

	__END_CATCH
}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
std::string PacketIDSet::toString () const
	throw ()
{
	StringStream msg;

	msg << "PacketIDSet("
		<< "PlayerStatus:" << (int)m_PlayerStatus 
		<< "PacketID:";

	for ( PACKET_ID_SET::const_iterator itr = m_PacketIDSet.begin() ;
		  itr != m_PacketIDSet.end() ;
		  itr ++ ) {

		msg << (*itr) << " ";
	}

	msg << ")";

	return msg.toString();
}
