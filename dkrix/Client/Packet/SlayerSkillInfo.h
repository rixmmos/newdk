//////////////////////////////////////////////////////////////////////
// 
// Filename    : SlayerSkillInfo.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __SLAYER_SKILL_INFO_H__
#define __SLAYER_SKILL_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "PCSkillInfo.h"
#include "SubSlayerSkillInfo.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class SlayerSkillInfo;
//

//
//////////////////////////////////////////////////////////////////////

class SlayerSkillInfo : public PCSkillInfo {

public :
	
	// constructor
	SlayerSkillInfo () throw ();
	
	// destructor
	~SlayerSkillInfo () throw ();

public :

    
    virtual void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    virtual void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getSize () throw ();

	static uint getMaxSize () throw () { 
		return szBYTE + szSkillDomainType + szBYTE + ( SubSlayerSkillInfo::getMaxSize() * 255 );
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set New Skill
	bool isLearnNewSkill() const throw() { return m_bLearnNewSkill; }
	void setLearnNewSkill( bool NewSkill ) throw() { m_bLearnNewSkill = NewSkill; }

	// get /set DomainType
	SkillDomainType_t getDomainiType() const throw() { return m_DomainType; }
	void setDomainType( SkillDomainType_t DomainType ) { m_DomainType = DomainType; }

	// get / set ListNumber
	BYTE getListNum() const throw() { return m_ListNum; }
	void setListNum( BYTE ListNum ) throw() { m_ListNum = ListNum; }

	// add / delete / clear S List
	void addListElement( SubSlayerSkillInfo * pSubSlayerSkillInfo ) throw() { m_SubSlayerSkillInfoList.push_back( pSubSlayerSkillInfo ); }

	// ClearList
	void clearList() throw() { m_SubSlayerSkillInfoList.clear(); m_ListNum = 0; }

	// pop front Element in Status List
	SubSlayerSkillInfo * popFrontListElement() throw() 
	{ 
		SubSlayerSkillInfo * TempSubSlayerSkillInfo = m_SubSlayerSkillInfoList.front(); m_SubSlayerSkillInfoList.pop_front(); return TempSubSlayerSkillInfo; 
	}

private :

	
	bool m_bLearnNewSkill;

	SkillDomainType_t m_DomainType;
	
	// SubSlayerSkillInfo List Total Number
	BYTE m_ListNum;

	// SubSlayerSkillInfo List
	std::list<SubSlayerSkillInfo*> m_SubSlayerSkillInfoList;

};

#endif
