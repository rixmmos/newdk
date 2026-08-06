//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCMineExplosionOK2.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MINE_EXPLOSION_OK_2_H__
#define __GC_MINE_EXPLOSION_OK_2_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCMineExplosionOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCMineExplosionOK2 : public Packet {

public :
	
	// constructor
	GCMineExplosionOK2 () throw ();
	
	// destructor
	~GCMineExplosionOK2 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_MINE_EXPLOSION_OK_2; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szCoord*2+ szDir + szItemType
	   + szBYTE + szObjectID * m_CListNum; }
         //CListNum, SListNum, ListEle * CListNum, ListEle * SListNum * 5 

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCMineExplosionOK2"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set X
	Coord_t getX() const throw() { return m_X; }
	void setX( Coord_t X ) throw() { m_X = X; }

	// get / set Y
	Coord_t getY() const throw() { return m_Y; }
	void setY( Coord_t Y ) throw() { m_Y = Y; }
	
	// get / set Dir
	Dir_t getDir() const throw() { return m_Dir; }
	void setDir( Dir_t r ) throw() { m_Dir = r; }
	
	// get / set ItemType
	ItemType_t getItemType() const throw() { return m_ItemType; }
	void setItemType( ItemType_t r ) throw() { m_ItemType = r; }

	void setXYDir( Coord_t X, Coord_t Y, Dir_t R) throw() { m_X = X; m_Y = Y; m_Dir = R;}

    // get / set Creature List Number
    BYTE getCListNum() const throw() { return m_CListNum; }
    void setCListNum( BYTE CListNum ) throw() { m_CListNum = CListNum; }


    // add / delete  Creature List
    void addCListElement( ObjectID_t ObjectID ) throw();

	// Clear Creature List
    void clearCList() throw() { m_CList.clear(); m_CListNum = 0; }

    // pop front Element in Status List
    ObjectID_t popCListElement() throw() { ObjectID_t CreatureList = m_CList.front(); m_CList.pop_front(); return CreatureList; }


private :
	
	// X , Y
	Coord_t m_X;

	Coord_t m_Y;

	// Dir
	Dir_t m_Dir;
	
	ItemType_t m_ItemType;

	// Creature List Num
	BYTE m_CListNum;

	// Creature List
	std::list<ObjectID_t> m_CList;

};


//////////////////////////////////////////////////////////////////////
//
// class GCMineExplosionOK2Factory;
//
// Factory for GCMineExplosionOK2
//
//////////////////////////////////////////////////////////////////////

class GCMineExplosionOK2Factory : public PacketFactory {

public :
	
	// constructor
	GCMineExplosionOK2Factory () throw () {}
	
	// destructor
	virtual ~GCMineExplosionOK2Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCMineExplosionOK2(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCMineExplosionOK2"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_MINE_EXPLOSION_OK_2; }

	// get Pakcet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szCoord*2 + szDir + szItemType 
		+ szBYTE + szWORD + szObjectID + 255; }
};


//////////////////////////////////////////////////////////////////////
//
// class GCMineExplosionOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCMineExplosionOK2Handler {

public :

	// execute packet's handler
	static void execute ( GCMineExplosionOK2 * pGCMineExplosionOK2 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
