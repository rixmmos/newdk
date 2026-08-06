//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAttackArmsOK1.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ATTACK_ARMS_OK_1_H__
#define __GC_ATTACK_ARMS_OK_1_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../ModifyInfo.h"
#include "../PacketFactory.h"
//#include "ModifyItemInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK1;
//

//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK1 : public ModifyInfo {

public :
	
	// constructor
	GCAttackArmsOK1 () throw ();
	
	// destructor
	~GCAttackArmsOK1 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_ATTACK_ARMS_OK_1; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBullet + szbool + ModifyInfo::getPacketSize() +szSkillType ;}

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCAttackArmsOK1"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set Success
	bool getSkillSuccess() const throw() { return m_bSuccess; }
	void setSkillSuccess( bool Success ) throw() { m_bSuccess = Success; }
	
	// get / set ObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get / set Bullet Num
	Bullet_t getBullet() const throw() { return m_BulletNum; }
	void setBulletNum( Bullet_t BulletNum ) { m_BulletNum = BulletNum; }

	void setSkillType( SkillType_t		SkillType ) { m_SkillType = SkillType; }
	SkillType_t getSkillType() const throw() { return m_SkillType; }		

private :

	// ObjectID
	ObjectID_t m_ObjectID;

	// Bullet Num
	Bullet_t m_BulletNum;

	
	bool m_bSuccess;
	SkillType_t		m_SkillType;

};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK1Factory;
//
// Factory for GCAttackArmsOK1
//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK1Factory : public PacketFactory {

public :
	
	// constructor
	GCAttackArmsOK1Factory () throw () {}
	
	// destructor
	virtual ~GCAttackArmsOK1Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCAttackArmsOK1(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCAttackArmsOK1"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_ATTACK_ARMS_OK_1; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szSkillType + szObjectID + szBullet + szbool + ModifyInfo::getPacketMaxSize(); }

};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK1Handler;
//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK1Handler {

public :

	// execute packet's handler
	static void execute ( GCAttackArmsOK1 * pGCAttackArmsOK1 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
