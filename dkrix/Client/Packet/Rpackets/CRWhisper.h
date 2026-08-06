//////////////////////////////////////////////////////////////////////
// 
// Filename    : CRWhisper.h 
// Written By  : elca
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CR_WHISPER_H__
#define __CR_WHISPER_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "WhisperManager.h"

//////////////////////////////////////////////////////////////////////
//
// class CRWhisper;
//



//
//////////////////////////////////////////////////////////////////////

class CRWhisper : public Packet {

public :

	CRWhisper();
	~CRWhisper();
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CR_WHISPER; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw ();	 

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CRWhisper"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	
	std::string getName() const throw () { return m_Name ; }
	void setName ( const std::string & Name ) throw () { m_Name = Name ; }

	
	std::string getTargetName() const throw () { return m_TargetName ; }
	void setTargetName ( const std::string & Name ) throw () { m_TargetName = Name ; }

	// get / set ListNumber
	BYTE getMessageSize() const throw() { return m_Messages.size(); }
	
	// get/set chatting message
	WHISPER_MESSAGE*	popMessage () throw ()	{ WHISPER_MESSAGE* pStr = m_Messages.front(); m_Messages.pop_front(); return pStr; }	
	void addMessage ( const WHISPER_MESSAGE & msg )	{ WHISPER_MESSAGE* pStr = new WHISPER_MESSAGE; *pStr = msg; m_Messages.push_back( pStr ); }

	// race
	bool isVampire () const throw () { return m_eRace == RACE_VAMPIRE; }
	bool isSlayer () const throw () { return m_eRace == RACE_VAMPIRE; }
	bool isOusters () const throw () { return m_eRace == RACE_OUSTERS; }
	Race_t getRace() const throw () { return m_eRace; }
	void setRace (Race eRace) throw () { m_eRace = eRace; }

	// World
	void setWorldID(const WorldID_t id)	{ m_WorldID = id; }
	const WorldID_t getWorldID() const	{ return m_WorldID; }

private :
	
	// character's creature Name
	std::string m_Name;

	std::string m_TargetName;
	
	// chatting message
	std::list<WHISPER_MESSAGE*> m_Messages;

	// Race
	Race_t m_eRace;

	// World
	WorldID_t	m_WorldID;

	
};


//////////////////////////////////////////////////////////////////////
//
// class CRWhisperFactory;
//
// Factory for CRWhisper
//
//////////////////////////////////////////////////////////////////////

class CRWhisperFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CRWhisper(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CRWhisper"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CR_WHISPER; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE + 10 + szBYTE + 10 + szBYTE + 32768 + szBYTE + szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class CRWhisperHandler;
//
//////////////////////////////////////////////////////////////////////

class CRWhisperHandler {
	
public :
	
	// execute packet's handler
	static void execute ( CRWhisper * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
