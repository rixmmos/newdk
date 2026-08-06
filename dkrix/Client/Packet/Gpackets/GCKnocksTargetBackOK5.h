//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCKnocksTargetBackOK5.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_KNOCKS_TARGET_BACK_OK_5_H__
#define __GC_KNOCKS_TARGET_BACK_OK_5_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK5;
//

//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK5 : public Packet {

public :
	
	// constructor
	GCKnocksTargetBackOK5 () throw ();
	
	// destructor
	~GCKnocksTargetBackOK5 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_KNOCKS_TARGET_BACK_OK_5; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szObjectID + szbool + szSkillType + szCoord*2 + szDir; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCKnocksTargetBackOK5"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set ObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get / set ObjectID
	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID( ObjectID_t TargetObjectID ) throw() { m_TargetObjectID = TargetObjectID; }
//	Coord_t getTargetX() const throw() { return m_X;}
//	Coord_t getTargetY() const throw() { return m_Y;}
//	void setTargetXY( Coord_t X, Coord_t Y) throw() { m_X = X; m_Y = Y;}

	// get / set success
	bool getSkillSuccess() const throw() { return m_bSuccess;}
	void setSkillSuccess( bool bSuccess) throw() { m_bSuccess = bSuccess;}

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
//	Coord_t m_X, m_Y;

	// TargetObjectID
	ObjectID_t m_TargetObjectID;

	// bool
	bool m_bSuccess;

    Coord_t m_X, m_Y;   
    Dir_t m_dir;        
    SkillType_t m_SkillType;

};


//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK5Factory;
//
// Factory for GCKnocksTargetBackOK5
//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK5Factory : public PacketFactory {

public :
	
	// constructor
	GCKnocksTargetBackOK5Factory () throw () {}
	
	// destructor
	virtual ~GCKnocksTargetBackOK5Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCKnocksTargetBackOK5(); }

	// get packet name
	std::string getPacketName () const throw () { return "GCKnocksTargetBackOK5"; }
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_KNOCKS_TARGET_BACK_OK_5; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szObjectID+ szbool + szSkillType + szCoord*2 + szDir; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK5Handler;
//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK5Handler {

public :

	// execute packet's handler
	static void execute ( GCKnocksTargetBackOK5 * pGCKnocksTargetBackOK5 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
