//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddVampire.h 
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_VAMPIRE_H__
#define __GC_ADD_VAMPIRE_H__

#include "../Packet.h"
#include "../PacketFactory.h"
#include "../PCVampireInfo3.h"
#include "../EffectInfo.h"
#include "../PetInfo.h"
#include "../NicknameInfo.h"
#include "../StoreInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddVampire;
//







//////////////////////////////////////////////////////////////////////////////

class GCAddVampire : public Packet 
{
public:
	GCAddVampire() throw() : m_pEffectInfo(NULL), m_pPetInfo(NULL), m_pNicknameInfo(NULL) { m_FromFlag = 0; }
	GCAddVampire(const PCVampireInfo3 & vampireInfo) throw() : m_VampireInfo(vampireInfo), m_pEffectInfo(NULL), m_pPetInfo(NULL), m_pNicknameInfo(NULL) { m_FromFlag = 0; }
	virtual ~GCAddVampire() throw();
	
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_ADD_VAMPIRE; }
	PacketSize_t getPacketSize() const throw()
	{
		PacketSize_t ret = m_VampireInfo.getSize() + m_pEffectInfo->getSize() + ((m_pPetInfo!=NULL)?m_pPetInfo->getSize():szPetType) + szBYTE;

		if ( m_pNicknameInfo == NULL )
		{
			NicknameInfo noNick;
			noNick.setNicknameType( NicknameInfo::NICK_NONE );
			ret += noNick.getSize();
		}
		else
		{
			ret += m_pNicknameInfo->getSize();
		}

		ret += m_StoreOutlook.getSize();

		return ret;
	}
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCAddVampire"; }
	string toString() const throw();
#endif
public:
	PCVampireInfo3 & getVampireInfo() throw() { return m_VampireInfo; }
	const PCVampireInfo3 & getVampireInfo() const throw() { return m_VampireInfo; }
	void setVampireInfo(const PCVampireInfo3 & vampireInfo) throw() { m_VampireInfo = vampireInfo; }

	EffectInfo* getEffectInfo() const throw() { return m_pEffectInfo; }
	void setEffectInfo(EffectInfo* pEffectInfo) throw() { m_pEffectInfo = pEffectInfo; }

	PetInfo* getPetInfo() const throw() { return m_pPetInfo; }
	void setPetInfo(PetInfo* pPetInfo) throw() { m_pPetInfo = pPetInfo; }

	NicknameInfo* getNicknameInfo() const throw() { return m_pNicknameInfo; }
	void setNicknameInfo(NicknameInfo* pNicknameInfo) throw() { m_pNicknameInfo = pNicknameInfo; }

	StoreOutlook	getStoreOutlook() const { return m_StoreOutlook; }
	void		setStoreInfo(StoreInfo* pInfo) { pInfo->makeStoreOutlook(m_StoreOutlook); }

	BYTE getFromFlag(void) const { return m_FromFlag; }
	void setFromFlag(BYTE flag) { m_FromFlag = flag; }

private:
	PCVampireInfo3 m_VampireInfo; 
	EffectInfo*    m_pEffectInfo; 
	PetInfo*	   m_pPetInfo;	  
	NicknameInfo*	   m_pNicknameInfo;	  
	StoreOutlook		m_StoreOutlook;	
	BYTE           m_FromFlag;    
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddVampireFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddVampireFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCAddVampire(); }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCAddVampire"; }
#endif
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_ADD_VAMPIRE; }
	PacketSize_t getPacketMaxSize() const throw() { return PCVampireInfo3::getMaxSize() + EffectInfo::getMaxSize() + PetInfo::getMaxSize() + NicknameInfo::getMaxSize() + StoreOutlook::getMaxSize() + szBYTE; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddVampireHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAddVampireHandler 
{
public:
	static void execute(GCAddVampire* pPacket, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
