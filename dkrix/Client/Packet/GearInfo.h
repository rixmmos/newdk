//////////////////////////////////////////////////////////////////////
// 
// Filename    : GearInfo.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GEAR_INFO_H__
#define __GEAR_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "GearSlotInfo.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class GearInfo;
//

//
//////////////////////////////////////////////////////////////////////

class GearInfo {

public :
	
	// constructor
	GearInfo () throw ();
	
	// destructor
	~GearInfo () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getSize () throw ();

	static uint getMaxSize () throw () { 
		return szBYTE + ( GearSlotInfo::getMaxSize() * 13 );
	}

	// get packet's debug std::string
	std::string toString () const throw ();

	// get / set ListNumber
	BYTE getListNum() const throw() { return m_ListNum; }
	void setListNum( BYTE ListNum ) throw() { m_ListNum = ListNum; }

	// add / delete / clear S List
	void addListElement( GearSlotInfo * pGearSlotInfo ) throw() { m_GearSlotInfoList.push_back( pGearSlotInfo ); }

	// ClearList
	void clearList() throw() { m_GearSlotInfoList.clear(); m_ListNum = 0; }

	// pop front Element in Status List
	GearSlotInfo * popFrontListElement() throw() 
	{ 
		GearSlotInfo * TempGearSlotInfo = m_GearSlotInfoList.front(); m_GearSlotInfoList.pop_front(); return TempGearSlotInfo; 
	}

private :
	
	// GearSlotInfo List Total Number
	BYTE m_ListNum;

	// GearSlotInfo List
	std::list<GearSlotInfo*> m_GearSlotInfoList;

};

#endif
