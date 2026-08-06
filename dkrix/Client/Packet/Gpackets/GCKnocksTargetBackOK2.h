//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCKnocksTargetBackOK2.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_KNOCKS_TARGET_BACK_OK_2_H__
#define __GC_KNOCKS_TARGET_BACK_OK_2_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../ModifyInfo.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK2 : public ModifyInfo {

public :
	
	// constructor
	GCKnocksTargetBackOK2 () throw ();
	
	// destructor
	~GCKnocksTargetBackOK2 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_KNOCKS_TARGET_BACK_OK_2; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szSkillType + szCoord*2 + szDir + szObjectID + ModifyInfo::getPacketSize(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCKnocksTargetBackOK2"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set CEffectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

    void setXYDir( Coord_t x, Coord_t y, Coord_t dir) throw()
    {
        m_X = x; m_Y = y; m_dir = dir;
    }
    Coord_t getX() const throw() { return m_X;}
    Coord_t getY() const throw() { return m_Y;}
    Dir_t getDir() const throw() { return m_dir;}
	SkillType_t getSkillType() const throw() { return m_SkillType;}
	void setSkillType( SkillType_t s) throw() { m_SkillType = s;}

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

    Coord_t m_X, m_Y;   
    Dir_t m_dir;        
    SkillType_t m_SkillType;

};


//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK2Factory;
//
// Factory for GCKnocksTargetBackOK2
//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK2Factory : public PacketFactory {

public :
	
	// constructor
	GCKnocksTargetBackOK2Factory () throw () {}
	
	// destructor
	virtual ~GCKnocksTargetBackOK2Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCKnocksTargetBackOK2(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCKnocksTargetBackOK2"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_KNOCKS_TARGET_BACK_OK_2; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szSkillType + szCoord*2 + szDir + szObjectID + ModifyInfo::getPacketMaxSize(); }

};


//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK2Handler {

public :

	// execute packet's handler
	static void execute ( GCKnocksTargetBackOK2 * pGCKnocksTargetBackOK2 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
