//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCChangeShape.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CHANGE_SHAPE_H__
#define __GC_CHANGE_SHAPE_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

#define SHAPE_FLAG_QUEST			0x01

//////////////////////////////////////////////////////////////////////
//
// class GCChangeShape;
//


//
//////////////////////////////////////////////////////////////////////

class GCChangeShape : public Packet {

public :
	
	// constructor
	GCChangeShape () throw ();
	
	// destructor
	~GCChangeShape () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_CHANGE_SHAPE; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBYTE + szItemType + szOptionType + szSpeed + szBYTE ; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCChangeShape"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get Object ID 
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( ObjectID_t objectID ) throw () { m_ObjectID = objectID; }

	// get/set ItemClass
	BYTE getItemClass() const throw() { return m_ItemClass; }
	void setItemClass( BYTE ItemClass ) throw() { m_ItemClass = ItemClass; }

	// get / set ItemType
	ItemType_t getItemType() const throw() { return m_ItemType; }
	void setItemType( ItemType_t ItemType ) throw() { m_ItemType = ItemType ; }

	// get / set OptionType
	OptionType_t getOptionType() const throw() { return m_OptionType; }
	void setOptionType( OptionType_t OptionType ) throw() { m_OptionType = OptionType ; }

	// get / set OptionType
	Speed_t getAttackSpeed() const throw() { return m_AttackSpeed; }
	void setAttackSpeed( Speed_t AttackSpeed ) throw() { m_AttackSpeed = AttackSpeed; }

	
	BYTE	getFlag()	const throw() { return m_Flag; }
	void	setFlag(BYTE flag)	throw() { m_Flag = flag; }

private :

	
	ObjectID_t m_ObjectID;
	
	// Item Class
	BYTE m_ItemClass;

	// Item Type
	ItemType_t m_ItemType;

	// Option Type
	OptionType_t m_OptionType;

	// Attack Speed
	Speed_t m_AttackSpeed;

	BYTE	m_Flag;

};


//////////////////////////////////////////////////////////////////////
//
// class GCChangeShapeFactory;
//
// Factory for GCChangeShape
//
//////////////////////////////////////////////////////////////////////

class GCChangeShapeFactory : public PacketFactory {

public :
	
	// constructor
	GCChangeShapeFactory () throw () {}
	
	// destructor
	virtual ~GCChangeShapeFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCChangeShape(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCChangeShape"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_CHANGE_SHAPE; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szBYTE + szItemType + szOptionType + szSpeed + szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCChangeShapeHandler;
//
//////////////////////////////////////////////////////////////////////

class GCChangeShapeHandler {

public :

	// execute packet's handler
	static void execute ( GCChangeShape * pGCChangeShape , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
