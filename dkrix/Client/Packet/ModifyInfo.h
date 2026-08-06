//////////////////////////////////////////////////////////////////////////////
// Filename    : ModifyInfo.h 
// Written By  : elca@ewestsoft.com
// Description :



//////////////////////////////////////////////////////////////////////////////

#ifndef __MODIFY_INFO_H__
#define __MODIFY_INFO_H__

#include "Packet/Types.h"
#include "Exception.h"
#include "Packet.h"
#include <list>

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
 
const std::string ModifyType2String[] = 
{
	"BASIC_STR",
	"CURRENT_STR",
	"MAX_STR",
	"STR_EXP",
	"BASIC_DEX",
	"CURRENT_DEX",
	"MAX_DEX",
	"DEX_EXP",
	"BASIC_INT",
	"CURRENT_INT",
	"MAX_INT",
	"INT_EXP",
	"CURRENT_HP",
	"MAX_HP",
	"CURRENT_MP",
	"MAX_MP",
	"MIN_DAMAGE",
	"MAX_DAMAGE",
	"DEFENSE",
	"PROTECTION",
	"TOHIT",
	"VISION",
	"FAME",
	"GOLD",
	"SWORD_DOMAIN_LEVEL",
	"SWORD_DOMAIN_EXP",
	"SWORD_DOMAIN_GOAL_EXP",
	"BLADE_DOMAIN_LEVEL",
	"BLADE_DOMAIN_EXP",
	"BLADE_DOMAIN_GOAL_EXP",
	"HEAL_DOMAIN_LEVEL",
	"HEAL_DOMAIN_EXP",
	"HEAL_DOMAIN_GOAL_EXP",
	"ENCHANT_DOMAIN_LEVEL",
	"ENCHANT_DOMAIN_EXP",
	"ENCHANT_DOMAIN_GOAL_EXP",
	"GUN_DOMAIN_LEVEL",
	"GUN_DOMAIN_EXP",
	"GUN_DOMAIN_GOAL_EXP",
	"ETC_DOMAIN_LEVEL",
	"ETC_DOMAIN_EXP",
	"ETC_DOMAIN_GOAL_EXP",
	"SKILL_LEVEL",
	"LEVEL",
	"EFFECT_STAT",
	"DURATION",
	"BULLET",
	"BONUS_POINT",
	"DURABILITY",
	"NOTORIETY",
	"VAMP_EXP",
	"SILVER_DAMAGE",
	"ATTACK_SPEED",
	"ALIGNMENT",
	"SILVER_DURABILITY",
	"REGEN_RATE",
	"GUILDID",
	"RANK",
	"RANK_EXP",
	"MODIFY_OUSTERS_EXP",
	"MODIFY_SKILL_BONUS_POINT",
    "MODIFY_SKILL_EXP",            

    "MODIFY_PET_HP",
    "MODIFY_PET_EXP",               
	
	"MODIFY_PET_TARGET",

    "MAX"
};

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
typedef struct _SHORTDATA
{
	BYTE   type;
	ushort value;

} SHORTDATA;

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
typedef struct _LONGDATA
{
	BYTE  type;
	ulong value;

} LONGDATA;


//////////////////////////////////////////////////////////////////////////////
// class ModifyInfo;


//////////////////////////////////////////////////////////////////////////////

class ModifyInfo : public Packet
{
public:
	ModifyInfo () throw ();
	virtual ~ModifyInfo () throw ();
	
public:
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);
	PacketSize_t getPacketSize () const throw () { return szBYTE*2 + m_ShortCount*(szBYTE+szshort) + m_LongCount*(szBYTE+szlong); }
	static PacketSize_t getPacketMaxSize() throw() { return szBYTE*2 + 255*(szBYTE+szshort+szBYTE+szlong); }

#ifdef __DEBUG_OUTPUT__
	std::string toString () const throw ();
#endif

public:
	BYTE getShortCount(void) const throw() { return m_ShortCount; }
	BYTE getLongCount(void) const throw() { return m_LongCount; }

	void addShortData(ModifyType type, ushort value) throw();
	void addLongData(ModifyType type, ulong value) throw();

	void popShortData(SHORTDATA& rData) throw();
	void popLongData(LONGDATA& rData) throw();

	void clearList(void) throw() { m_ShortCount = 0; m_LongCount = 0; m_ShortList.clear(); m_LongList.clear(); }

protected:
	BYTE            m_ShortCount;
	std::list<SHORTDATA> m_ShortList;

	BYTE            m_LongCount;
	std::list<LONGDATA>  m_LongList;
};

#endif
