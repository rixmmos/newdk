//////////////////////////////////////////////////////////////////////////////
// Filename    : GCShopBought.h 

// Description : 





//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_SHOP_BOUGHT_H__
#define __GC_SHOP_BOUGHT_H__

#include "../Packet.h"
#include "../PacketFactory.h"
#include <list>

//////////////////////////////////////////////////////////////////////////////
// class GCShopBought;
//////////////////////////////////////////////////////////////////////////////

class GCShopBought : public Packet 
{
public:
	GCShopBought() throw();
	virtual ~GCShopBought() throw();
	
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_SHOP_BOUGHT; }
	PacketSize_t getPacketSize() const throw() 
	{ 
		return szObjectID +   // NPC OID
			szShopVersion +   // shop version
			szShopRackType +  // shop rack type
			szBYTE +          // shop rack index
			szObjectID +      // item OID
			szBYTE +          // item class
			szItemType +      // item type
			szBYTE + m_OptionType.size() +    // item option type
			szDurability +    // item durability
			szSilver +        // silver coating amount
			szGrade +         // grade
			szEnchantLevel;   // enchant level
	}

#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const throw() { return "GCShopBought"; }
	std::string toString() const throw();
#endif

public:
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t creatureID) throw() { m_ObjectID = creatureID; }

	ShopVersion_t getShopVersion(void) const throw() { return m_Version;}
	void setShopVersion(const ShopVersion_t ver) throw() { m_Version = ver;}

	ShopRackType_t getShopType(void) const throw() { return m_ShopType;}
	void setShopType(ShopRackType_t type) throw() { m_ShopType = type;}

	BYTE getShopIndex(void) const throw() { return m_ShopIndex;}
	void setShopIndex(BYTE index) throw() { m_ShopIndex = index;}
	
	ObjectID_t getItemObjectID(void) const throw() { return m_ItemObjectID;}
	void setItemObjectID(ObjectID_t oid) throw() { m_ItemObjectID = oid;}
	
	int getItemClass(void) const throw() { return m_ItemClass;}
	void setItemClass(int iclass) throw() { m_ItemClass = iclass;}
	
	ItemType_t getItemType(void) const throw() { return m_ItemType;}
	void setItemType(ItemType_t type) throw() { m_ItemType = type;}
	
	int getOptionTypeSize(void) const throw() { return m_OptionType.size();}
	const std::list<OptionType_t>& getOptionType() const throw() { return m_OptionType; }
	OptionType_t popOptionType(void) throw()
	{
		if (m_OptionType.empty()) return 0;
		OptionType_t optionType = m_OptionType.front();
		m_OptionType.pop_front();
		return optionType;
	}
	void addOptionType(OptionType_t type) throw() { m_OptionType.push_back( type ); }
	void setOptionType(const std::list<OptionType_t>& OptionTypes) throw() { m_OptionType = OptionTypes; }
	
	Durability_t getDurability(void) const throw() { return m_Durability;}
	void setDurability(Durability_t dur) throw() { m_Durability = dur;}

	Silver_t getSilver(void) const throw() { return m_Silver; }
	void setSilver(Silver_t silver) throw() { m_Silver = silver; }

	Grade_t getGrade(void) const throw() { return m_Grade; }
	void setGrade(Grade_t Grade) throw() { m_Grade = Grade; }

	EnchantLevel_t getEnchantLevel(void) const throw() { return m_EnchantLevel; }
	void setEnchantLevel(EnchantLevel_t level) throw() { m_EnchantLevel = level; }
	

private:
	ObjectID_t      		m_ObjectID;     // NPC's object id
	ShopVersion_t   		m_Version;      
	ShopRackType_t  		m_ShopType;	    
	BYTE            		m_ShopIndex;    
	ObjectID_t      		m_ItemObjectID; 
	BYTE            		m_ItemClass;    
	ItemType_t      		m_ItemType;     
	std::list<OptionType_t>    	m_OptionType;   
	Durability_t    		m_Durability;   
	Silver_t        		m_Silver;       
	Grade_t        			m_Grade;       
	EnchantLevel_t  		m_EnchantLevel; 
	
};


//////////////////////////////////////////////////////////////////////////////
// class GCShopBoughtFactory;
//////////////////////////////////////////////////////////////////////////////

class GCShopBoughtFactory : public PacketFactory 
{
public :
	Packet* createPacket() throw() { return new GCShopBought(); }
	std::string getPacketName() const throw() { return "GCShopBought"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SHOP_BOUGHT; }
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return szObjectID +  // NPC OID
			szShopVersion +  // shop version
			szShopRackType + // shop rack type
			szBYTE +         // shop rack index
   		 	szObjectID +     // item OID
			szBYTE +         // item class
			szItemType +     // item type
			szBYTE + 255 +   // item option type
			szDurability +   // item durablility
			szSilver +       // silver coating amount
			szGrade +       // silver coating amount
			szEnchantLevel;  // enchant level
	}
};


//////////////////////////////////////////////////////////////////////////////
// class GCShopBoughtHandler;
//////////////////////////////////////////////////////////////////////////////

class GCShopBoughtHandler 
{
public:
	static void execute(GCShopBought* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
