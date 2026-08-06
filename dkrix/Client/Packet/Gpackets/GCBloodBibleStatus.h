//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCBloodBibleStatus.h 
// Written By  : Reiot
// 
//////////////////////////////////////////////////////////////////////
//




//
//////////////////////////////////////////////////////////////////////


#ifndef __GC_BLOOD_BIBLE_STATUS_H__
#define __GC_BLOOD_BIBLE_STATUS_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCBloodBibleStatus;
//



//
//////////////////////////////////////////////////////////////////////

class GCBloodBibleStatus : public Packet {

public :
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_BLOOD_BIBLE_STATUS; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() 
	{ 
		return szItemType 
				+ szZoneID 
				+ szStorage 
				+ szBYTE + m_OwnerName.size() 
				+ szRace 
				+ szRace 
				+ szZoneCoord + szZoneCoord;
	}

	// get packet name
	std::string getPacketName() const throw() { return "GCBloodBibleStatus"; }
	
	// get packet's debug std::string
	std::string toString() const throw();

	// get/set text color
	ItemType_t getItemType() const throw() { return m_ItemType; }
	void setItemType( ItemType_t itemType ) throw() { m_ItemType = itemType; }

	// get/set text color
	ZoneID_t getZoneID() const throw() { return m_ZoneID; }
	void setZoneID( ZoneID_t zoneID ) throw() { m_ZoneID = zoneID; }

	// get/set text color
	Storage_t getStorage() const throw() { return m_Storage; }
	void setStorage( Storage_t storage ) throw() { m_Storage = storage; }

	// get/set chatting message
	const std::string& getOwnerName() const throw() { return m_OwnerName; }
	void setOwnerName(const std::string & OwnerName) throw() { m_OwnerName = OwnerName; }

	// get/set text color
	Race_t getRace() const throw() { return m_Race; }
	void setRace( Race_t race ) throw() { m_Race = race; }

	// get/set text color
	Race_t getShrineRace() const throw() { return m_ShrineRace; }
	void setShrineRace( Race_t race ) throw() { m_ShrineRace = race; }

	// get/set text color
	ZoneCoord_t getX() const throw() { return m_X; }
	void setX( ZoneCoord_t x ) throw() { m_X = x; }

	// get/set text color
	ZoneCoord_t getY() const throw() { return m_Y; }
	void setY( ZoneCoord_t y ) throw() { m_Y = y; }

private :
	
	ItemType_t 		m_ItemType;	

	ZoneID_t		m_ZoneID; 

	Storage_t 		m_Storage;
	std::string 			m_OwnerName;
	Race_t			m_Race;
	Race_t			m_ShrineRace;
	ZoneCoord_t		m_X;
	ZoneCoord_t		m_Y; 
};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodBibleStatusFactory;
//
// Factory for GCBloodBibleStatus
//
//////////////////////////////////////////////////////////////////////

class GCBloodBibleStatusFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCBloodBibleStatus(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCBloodBibleStatus"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_BLOOD_BIBLE_STATUS; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return szItemType 
				+ szZoneID 
				+ szStorage 
				+ szBYTE + 255
				+ szRace 
				+ szRace 
				+ szZoneCoord + szZoneCoord;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCBloodBibleStatusHandler;
//
//////////////////////////////////////////////////////////////////////

class GCBloodBibleStatusHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCBloodBibleStatus* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
