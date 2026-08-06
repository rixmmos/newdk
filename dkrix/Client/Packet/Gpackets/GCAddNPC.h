//--------------------------------------------------------------------------------
// 
// Filename    : GCAddNPC.h 
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

#ifndef __GC_ADD_NPC_H__
#define __GC_ADD_NPC_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCAddNPC;
//

//
//--------------------------------------------------------------------------------

class GCAddNPC : public Packet 
{

public :
	
	
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
			
	
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_ADD_NPC; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szObjectID 
			+ szBYTE + m_Name.size() + szNPCID
			+ szSpriteType 
			+ szColor + szColor
			+ szCoord + szCoord + szDir;
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCAddNPC"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


public :

	// get/set object id
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( ObjectID_t creatureID ) throw () { m_ObjectID = creatureID; }

	// get/set name
	std::string getName () const throw () { return m_Name; }
	void setName ( std::string name ) throw () { m_Name = name; }

	// get/set NPC id
	NPCID_t getNPCID(void) const throw() { return m_NPCID;}
	void setNPCID(NPCID_t NPCID) throw() { m_NPCID = NPCID;}

	// get/set sprite type
	SpriteType_t getSpriteType () const throw () { return m_SpriteType; }
	void setSpriteType ( SpriteType_t spriteType ) throw () { m_SpriteType = spriteType; }

	// get/set main color
	Color_t getMainColor () const throw () { return m_MainColor; }
	void setMainColor ( Color_t color ) throw () { m_MainColor = color; }

	// get/set sub color
	Color_t getSubColor () const throw () { return m_SubColor; }
	void setSubColor ( Color_t color ) throw () { m_SubColor = color; }

	// get/set X
	Coord_t getX () const throw () { return m_X; }
	void setX ( Coord_t x ) throw () { m_X = x; }
	
	// get/set Y
	Coord_t getY () const throw () { return m_Y; }
	void setY ( Coord_t y ) throw () { m_Y = y; }

	// get/set Dir
	Dir_t getDir () const throw () { return m_Dir; }
	void setDir ( Dir_t dir ) throw () { m_Dir = dir; }

private :
	
	
	ObjectID_t m_ObjectID;

	
	std::string m_Name;

	
	NPCID_t m_NPCID;

	
	SpriteType_t m_SpriteType;

	
	Color_t m_MainColor;
	Color_t m_SubColor;

	
	Coord_t m_X;
	Coord_t m_Y;
	Dir_t m_Dir;

};


//////////////////////////////////////////////////////////////////////
//
// class GCAddNPCFactory;
//
// Factory for GCAddNPC
//
//////////////////////////////////////////////////////////////////////

class GCAddNPCFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCAddNPC(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCAddNPC"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_ADD_NPC; }

	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () 
	{
		return szObjectID 
			+ szBYTE + 40 + szNPCID
			+ szSpriteType 
			+ szColor + szColor
			+ szCoord + szCoord + szDir;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCAddNPCHandler;
//
//////////////////////////////////////////////////////////////////////

class GCAddNPCHandler {

public :

	// execute packet's handler
	static void execute ( GCAddNPC * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
