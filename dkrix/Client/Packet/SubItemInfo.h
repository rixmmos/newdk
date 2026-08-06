//----------------------------------------------------------------------
//
// Filename    : SubItemInfo.h
// Written By  : elca
// Description :
//
//----------------------------------------------------------------------

#ifndef __SUB_ITEM_INFO_H__
#define __SUB_ITEM_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//----------------------------------------------------------------------
//

//


//
//----------------------------------------------------------------------

class SubItemInfo {

public :

	// read data from socket input stream
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );

	// write data to socket output stream
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// get size of object
	static uint getSize () throw ()
	{ return szObjectID + szBYTE + szItemType + szItemNum + szSlotID; }

	// get max size of object
	static uint getMaxSize () throw ()
	{ return szObjectID + szBYTE + szItemType + szItemNum + szSlotID; }

	// get debug std::string
	std::string toString () const throw ();

public :

	// get/set ObjectID
	void setObjectID( ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }

	// get/set ItemClass
	void setItemClass ( BYTE IClass ) throw() { m_IClass = IClass; }
	BYTE getItemClass () const throw() { return m_IClass; }

	// get/set ItemType
	void setItemType ( ItemType_t ItemType ) throw() { m_ItemType = ItemType; }
	ItemType_t getItemType() const throw() { return m_ItemType; }

	// get/set ItemNum
	void setItemNum ( ItemNum_t ItemNum ) throw() { m_ItemNum = ItemNum; }
	ItemNum_t getItemNum() const throw() { return m_ItemNum; }

	// set/set SlotID
	void setSlotID( SlotID_t SlotID ) throw() { m_SlotID = SlotID; }
	SlotID_t getSlotID() const throw() { return m_SlotID; }

private :

	ObjectID_t m_ObjectID; 

	BYTE m_IClass;

	ItemType_t m_ItemType;

	ItemNum_t m_ItemNum;

	SlotID_t m_SlotID;

};

#endif
