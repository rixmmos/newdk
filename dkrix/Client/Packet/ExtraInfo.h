//////////////////////////////////////////////////////////////////////
// 
// Filename    : ExtraInfo.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __EXTRA_INFO_H__
#define __EXTRA_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "ExtraSlotInfo.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class ExtraInfo;
//

//
//////////////////////////////////////////////////////////////////////

class ExtraInfo {

public :
	
	// constructor
	ExtraInfo () throw ();
	
	// destructor
	~ExtraInfo () throw ();

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getSize () throw ();

	static uint getMaxSize () throw () { 
		return szBYTE + ( ExtraSlotInfo::getMaxSize() * 1 );
	}

	// get packet's debug std::string
	#ifdef __DEBUG_OUTPUT__
		std::string toString () const throw ();
	#endif

	// get / set ListNumber
	BYTE getListNum() const throw() { return m_ListNum; }
	void setListNum( BYTE ListNum ) throw() { m_ListNum = ListNum; }

	// add / delete / clear S List
	void addListElement( ExtraSlotInfo * pExtraSlotInfo ) throw() { m_ExtraSlotInfoList.push_back( pExtraSlotInfo ); }

	// ClearList
	void clearList() throw() { m_ExtraSlotInfoList.clear(); m_ListNum = 0; }

	// pop front Element in Status List
	ExtraSlotInfo * popFrontListElement() throw() 
	{ 
		ExtraSlotInfo * TempExtraSlotInfo = m_ExtraSlotInfoList.front(); m_ExtraSlotInfoList.pop_front(); return TempExtraSlotInfo; 
	}

private :
	
	// ExtraSlotInfo List Total Number
	BYTE m_ListNum;

	// ExtraSlotInfo List
	std::list<ExtraSlotInfo*> m_ExtraSlotInfoList;

};

#endif
