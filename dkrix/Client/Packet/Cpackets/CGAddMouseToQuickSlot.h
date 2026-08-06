//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddMouseToQuickSlot.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_MOUSE_TO_QUICKSLOT_H__
#define __CG_ADD_MOUSE_TO_QUICKSLOT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGAddMouseToQuickSlot;
//
//////////////////////////////////////////////////////////////////////

class CGAddMouseToQuickSlot : public Packet {
public :

	// constructor
	CGAddMouseToQuickSlot() throw();

	// destructor
	~CGAddMouseToQuickSlot() throw();

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_ADD_MOUSE_TO_QUICKSLOT; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szSlotID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddMouseToQuickSlot"; }
	
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	// get / set ObjectID
	ObjectID_t getObjectID() const throw () { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get / set QuickSlotID
	SlotID_t getSlotID() const throw() { return m_SlotID; }
	void setSlotID( SlotID_t SlotID ) throw() { m_SlotID = SlotID; }


private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	
	SlotID_t m_SlotID;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAddMouseToQuickSlotFactory;
//
// Factory for CGAddMouseToQuickSlot
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGAddMouseToQuickSlotFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGAddMouseToQuickSlot(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGAddMouseToQuickSlot"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_ADD_MOUSE_TO_QUICKSLOT; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szSlotID; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGAddMouseToQuickSlotHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGAddMouseToQuickSlotHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGAddMouseToQuickSlot * pPacket , Player * player ) throw ( ProtocolException , Error );
	};

#endif
#endif
