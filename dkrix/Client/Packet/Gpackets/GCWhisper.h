//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCWhisper.h 
// Written By  : elca
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_WHISPER_H__
#define __GC_WHISPER_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCWhisper;
//



//
//////////////////////////////////////////////////////////////////////

class GCWhisper : public Packet {

public :
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_WHISPER; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE + m_Name.size() + szuint + szBYTE + m_Message.size() + szBYTE; }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCWhisper"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif

	// get/set sender's creature id
	std::string getName() const throw() { return m_Name ; }
	void setName(const std::string & Name) throw() { m_Name = Name ; }

	//get/set text color
	uint getColor() const throw() { return m_Color; }
	void setColor( uint color ) throw() { m_Color = color; }

	// get/set chatting message
	std::string getMessage() const throw() { return m_Message; }
	void setMessage(const std::string & msg) throw() { m_Message = msg; }
	
	// get/set chatting message
	Race_t getRace() const throw() { return m_Race; }
	void setRace(Race_t race) throw() { m_Race = race; }

private :
	
	// character's creature Name
	std::string m_Name;
	
	// text color
	uint m_Color;

	// chatting message
	std::string m_Message;

	// Race
	Race_t m_Race;
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCWhisperFactory;
//
// Factory for GCWhisper
//
//////////////////////////////////////////////////////////////////////

class GCWhisperFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCWhisper(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCWhisper"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_WHISPER; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + 10 + szuint + szBYTE + 128 + szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCWhisperHandler;
//
//////////////////////////////////////////////////////////////////////

class GCWhisperHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCWhisper* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
