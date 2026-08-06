//--------------------------------------------------------------------------------
// 
// Filename    : GCNPCInfo.h 
// Written By  : Reiot
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __GC_NPC_INFO_H__
#define __GC_NPC_INFO_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "../GameTime.h"
#include "../PCSlayerInfo2.h"
#include "../PCVampireInfo2.h"
#include "../InventoryInfo.h"
#include "../GearInfo.h"
#include "../ExtraInfo.h"
#include "../EffectInfo.h"
#include "../Assert.h"
#include "../RideMotorcycleInfo.h"
#include "../NPCInfo.h"

#define FLAG_PREMIUM_ZONE			0x10	
#define FLAG_PREMIUM_PLAY			0x01	

//--------------------------------------------------------------------------------
//
// class GCNPCInfo;
//



//
//--------------------------------------------------------------------------------

class GCNPCInfo : public Packet {

public :

	// constructor
	GCNPCInfo() throw();

	// destructor
	~GCNPCInfo() throw();
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_NPC_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() 
	{ 
		PacketSize_t size = 0;

		size += szBYTE;
		std::list<NPCInfo*>::const_iterator itr = m_NPCInfos.begin();
		for(; itr != m_NPCInfos.end(); itr++)
		{
			NPCInfo* pInfo = *itr;
			size += pInfo->getSize();
		}

		return size;
	}

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCNPCInfo"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif


//--------------------------------------------------
// methods
//--------------------------------------------------
public :
	// get/set npc info
	void addNPCInfo(NPCInfo* pInfo) { m_NPCInfos.push_back(pInfo);}
	NPCInfo* popNPCInfo(void) { if (m_NPCInfos.empty()) return NULL; NPCInfo* pInfo = m_NPCInfos.front(); m_NPCInfos.pop_front(); return pInfo; }

//--------------------------------------------------
// data members
//--------------------------------------------------
private :
	
	std::list<NPCInfo*> m_NPCInfos;
};


//--------------------------------------------------------------------------------
//
// class GCNPCInfoFactory;
//
// Factory for GCNPCInfo
//
//--------------------------------------------------------------------------------

class GCNPCInfoFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCNPCInfo(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCNPCInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_NPC_INFO; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		PacketSize_t size = 0;

		size += szBYTE;
		size += NPCInfo::getMaxSize()* 255;

		return size;
	}

};



//--------------------------------------------------------------------------------
//
// class GCNPCInfoHandler;
//
//--------------------------------------------------------------------------------

class GCNPCInfoHandler {

public :

	// execute packet's handler
	static void execute(GCNPCInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
