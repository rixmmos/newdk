//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCMakeItemOK.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MAKE_ITEM_OK_H__
#define __GC_MAKE_ITEM_OK_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"
#include "GCChangeInventoryItemNum.h"
#include "GCAddItemToInventory.h"
#include "../ModifyInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemOK;
//

//
//////////////////////////////////////////////////////////////////////

class GCMakeItemOK : public GCChangeInventoryItemNum, public GCAddItemToInventory, public ModifyInfo {

public :
	
	// constructor
	GCMakeItemOK () throw ();
	
	// destructor
	~GCMakeItemOK () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_MAKE_ITEM_OK; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return GCChangeInventoryItemNum::getPacketSize() 
		+ GCAddItemToInventory::getPacketSize()
		+ ModifyInfo::getPacketSize(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCMakeItemOK"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

private :

};


//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemOKFactory;
//
// Factory for GCMakeItemOK
//
//////////////////////////////////////////////////////////////////////

class GCMakeItemOKFactory : public PacketFactory {

public :
	
	// constructor
	GCMakeItemOKFactory () throw () {}
	
	// destructor
	virtual ~GCMakeItemOKFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCMakeItemOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCMakeItemOK"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_MAKE_ITEM_OK; }

	// get Packet Max Size
	// PacketSize_t getPacketMaxSize() const throw() { return szSkillType + szCEffectID + szDuration + szBYTE + szBYTE * m_ListNum * 2 ; }
	PacketSize_t getPacketMaxSize() const throw() { return  255 + 255 + ModifyInfo::getPacketMaxSize(); }
};


//////////////////////////////////////////////////////////////////////
//
// class GCMakeItemOKHandler;
//
//////////////////////////////////////////////////////////////////////

class GCMakeItemOKHandler {

public :

	// execute packet's handler
	static void execute ( GCMakeItemOK * pGCMakeItemOK , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
