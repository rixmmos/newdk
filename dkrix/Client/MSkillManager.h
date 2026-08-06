//----------------------------------------------------------------------
// MSkillManager.h
//----------------------------------------------------------------------
//
// < SKILLINFO_NODE >
//




//
//
// < SkillInfoTable >
//


//
//
// < MSkillDomain >
//





//
//
// < MSkillManager >
//

//
//----------------------------------------------------------------------
//
 
//----------------------------------------------------------------------

#ifndef	__MSKILLMANAGER_H__
#define	__MSKILLMANAGER_H__

#pragma warning(disable:4786)

#include "SkillDef.h"
#include "MString.h"
#include "CTypeTable.h"
#include "DrawTypeDef.h"
#include "ExpInfo.h"
#include "RaceType.h"

#include <fstream>
#include <list>
#include <map>
#include <vector>
using namespace std;



//----------------------------------------------------------------------
//
//		SKILLINFO_NODE
//
//----------------------------------------------------------------------
class SKILLINFO_NODE {
	public :
		enum ELEMENTAL_DOMAIN
		{
			ELEMENTAL_DOMAIN_NO_DOMAIN = -1,     
			ELEMENTAL_DOMAIN_FIRE,              
			ELEMENTAL_DOMAIN_WATER,             
			ELEMENTAL_DOMAIN_EARTH,             
			ELEMENTAL_DOMAIN_WIND,              
			ELEMENTAL_DOMAIN_COMBAT,            
			ELEMENTAL_DOMAIN_ELEMENTAL_COMBAT,  
			ELEMENTAL_DOMAIN_ETC,               
			
			ELEMENTAL_DOMAIN_MAX
		};

		typedef	std::list<ACTIONINFO>		SKILLID_LIST;				
		typedef	std::list<int>				SKILLTYPE_LIST;

	public :
		SKILLINFO_NODE();
		SKILLINFO_NODE(int level, const char* name, int x, int y, TYPE_SPRITEID sid, const char* hname)
		{
			Set(level, name, x,y, sid, hname);
		}

		~SKILLINFO_NODE() {}

		//------------------------------------------------------
		// Set
		//------------------------------------------------------
		void			Set(int level, const char* name, int x, int y, TYPE_SPRITEID sid, const char* hname)
		{
			m_Level		= level;
			m_Name		= name;
			m_X			= x;
			m_Y			= y;
			m_SpriteID	= sid;
			m_HName		= hname;
		}

		void			SetMP(int mp)				{ m_MP = mp; }

		//------------------------------------------------------
		// Add NextSkill
		//------------------------------------------------------
		bool			AddNextSkill(ACTIONINFO id);
		

		//------------------------------------------------------
		// Get
		//------------------------------------------------------
		int					GetLevel() const		{ return m_Level; }		
		const char*			GetName() const				{ return m_Name.GetString(); }
		const char*			GetHName() const			{ return m_HName.GetString(); }
		int					GetX() const				{ return m_X; }
		int					GetY() const				{ return m_Y; }
		TYPE_SPRITEID		GetSpriteID() const			{ return m_SpriteID; }
		const SKILLID_LIST& GetNextSkillList() const	{ return m_listNextSkill; }
		int					GetMP() const				{ return m_MP; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		void				SetExpLevel(int lev)		{ m_ExpLevel = lev; }
		int					GetExpLevel() const			{ return m_ExpLevel; }
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		bool			IsPassive() const	{ return m_bPassive; }
		void			SetPassive()		{ m_bPassive = true; }
		void			UnSetPassive()		{ m_bPassive = false; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		bool			IsActive() const	{ return m_bActive; }
		void			SetActive()			{ m_bActive = true; }
		void			UnSetActive()		{ m_bActive = false; }

		//------------------------------------------------------
		// skill exp
		//------------------------------------------------------
		void				SetSkillExp(int exp)		{ m_SkillExp = exp; }
		int					GetSkillExp() const			{ return m_SkillExp; }		

		//------------------------------------------------------
		// skill step
		//------------------------------------------------------
		void				SetSkillStep(SKILL_STEP step)	{ m_SkillStep = step; }
		SKILL_STEP			GetSkillStep() const			{ return m_SkillStep; }				

		//------------------------------------------------------
		// Available Level
		//------------------------------------------------------
		void				SetLearnLevel(int lv)	{ m_LearnLevel = lv; }
		int					GetLearnLevel() const	{ return m_LearnLevel; }						

		//------------------------------------------------------
		
		//------------------------------------------------------
		void				SetVampireSkill()		{ m_eSkillRace = RACE_VAMPIRE; }
		bool				IsVampireSkill()		{ return m_eSkillRace == RACE_VAMPIRE; }
		void				SetSlayerSkill()		{ m_eSkillRace = RACE_SLAYER; }
		bool				IsSlayerSkill()			{ return m_eSkillRace == RACE_SLAYER; }
		void				SetOustersSkill()		{ m_eSkillRace = RACE_OUSTERS; }
		bool				IsOustersSkill()		{ return m_eSkillRace == RACE_OUSTERS; }
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		void				SetDelayTime(DWORD delay);		
		DWORD				GetDelayTime() const	{ return m_DelayTime; }

		bool				IsAvailableTime() const;		
		void				SetAvailableTime(int delay = 0);				
		DWORD				GetAvailableTimeLeft() const;	
		
		void				SetNextAvailableTime();			

		//------------------------------------------------------
		
		//------------------------------------------------------
		bool				IsEnable() const		{ return m_bEnable; }
		void				SetEnable(bool enable=true);
		//void				SetDisable();

		//------------------------------------------------------
		// File I/O
		//------------------------------------------------------
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);

		//------------------------------------------------------
		// Load / Save SERVER_SKILLINFO
		//------------------------------------------------------
		void		LoadFromFileServerSkillInfo(std::ifstream& file);
		void        SaveFromFileServerSkillInfo(std::ofstream& file);

	protected :
		
		int				m_Level;			

		MString			m_Name;				
		MString			m_HName;			

		int				m_X, m_Y;			
		TYPE_SPRITEID	m_SpriteID;			

		int				m_MP;				

		SKILLID_LIST	m_listNextSkill;	

		bool			m_bPassive;			

		bool			m_bActive;			

		//------------------------------------------------------
		
		//------------------------------------------------------
		int				m_ExpLevel;			
		int				m_SkillExp;			

		//------------------------------------------------------
		
		//------------------------------------------------------
		SKILL_STEP		m_SkillStep;

		//------------------------------------------------------
		
		//------------------------------------------------------
		int				m_LearnLevel;

		//------------------------------------------------------
		
		//------------------------------------------------------
		Race			m_eSkillRace;

		//------------------------------------------------------
		
		//------------------------------------------------------
		DWORD			m_DelayTime;		
		DWORD			m_AvailableTime;	
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		bool			m_bEnable;	

	public :
		int			DomainType;		
		int			minDamage;		
		int			maxDamage;		
		int			minDelay;		
		int			maxDelay;		
		int			minCastTime;	
		int			maxCastTime;	
		int			minDuration;	
		int			maxDuration;	
		int			minRange;		
		int			maxRange;		
		int			maxExp;			
		
		
	public:
		int			SkillPoint;
		int			LevelUpPoint;
		SKILLTYPE_LIST		SkillTypeList;
		int			Fire;
		int			Water;
		int			Earth;
		int			Wind;
		int			Sum;
		int			Wristlet;
		int			Stone1;
		int			Stone2;
		int			Stone3;
		int			Stone4;
		int			ElementalDomain;
		BYTE		CanDelete; 
		
};

//----------------------------------------------------------------------
//
//	MSkillSet
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
const BYTE	FLAG_SKILL_ENABLE		= 1;		

class SKILLID_NODE {
	public :
		ACTIONINFO		SkillID;	
		BYTE			Flag;

	public :
		SKILLID_NODE(ACTIONINFO sid, BYTE f=FLAG_SKILL_ENABLE)
		{
			SkillID = sid;
			Flag	= f;
		}

		void	SetEnable()			{ Flag |= FLAG_SKILL_ENABLE; }
		void	SetDisable()		{ Flag &= ~FLAG_SKILL_ENABLE; }
		BYTE	IsEnable() const	{ return Flag & FLAG_SKILL_ENABLE; }

		void	operator = (const SKILLID_NODE& node)
		{
			SkillID		= node.SkillID;
			Flag		= node.Flag;
		}

};
		
class MSkillSet : public std::map<ACTIONINFO, SKILLID_NODE> {
	public :
		// <SkillID>
		typedef	std::map<ACTIONINFO, SKILLID_NODE>		SKILLID_MAP;
	

	public :
		MSkillSet() {}
		~MSkillSet() {}

		//------------------------------------------------------
		// Skill
		//------------------------------------------------------
		bool			AddSkill(ACTIONINFO id, BYTE flag=FLAG_SKILL_ENABLE);		
		bool			RemoveSkill(ACTIONINFO id);		
		
		//------------------------------------------------------
		// Enable?
		//------------------------------------------------------
		bool			IsEnableSkill(ACTIONINFO id) const;	
		bool			EnableSkill(ACTIONINFO id);		
		bool			DisableSkill(ACTIONINFO id);	

		//------------------------------------------------------
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		void			SetAvailableSkills();	
		void			CheckMP();				// mp check

		void			SetAvailableVampireSkills();

	
	protected :
};

//----------------------------------------------------------------------
//
//	MSkillDomain
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MSkillDomain {
	public :
		enum SKILLSTATUS
		{
			SKILLSTATUS_NULL = 0,		
			SKILLSTATUS_LEARNED,		
			SKILLSTATUS_NEXT,			
			SKILLSTATUS_OTHER			
		};

		
		typedef	std::map<ACTIONINFO, SKILLSTATUS>		SKILLID_MAP;
	
		// Skill Step List
		typedef std::vector<ACTIONINFO>					SKILL_STEP_LIST;

		// Skill Step Map
		typedef std::map<SKILL_STEP, SKILL_STEP_LIST*>	SKILL_STEP_MAP;


	public :
		MSkillDomain();
		~MSkillDomain();

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		bool		AddSkill(ACTIONINFO id);

		//------------------------------------------------------
		// Clear
		//------------------------------------------------------
		void			Clear();
		void			ClearSkillList();

		//------------------------------------------------------
		
		
		//------------------------------------------------------
		bool			SetRootSkill(ACTIONINFO id, bool reset = true);		

		//------------------------------------------------------
		// Set SkillStatus
		//------------------------------------------------------
		
		//bool			SetSkillStatus(ACTIONINFO id, SKILLSTATUS status);	

		//------------------------------------------------------
		// Get SkillStatus
		//------------------------------------------------------
		
		SKILLSTATUS		GetSkillStatus(ACTIONINFO id) const;	
		
		

		//------------------------------------------------------
		// New Skill
		//------------------------------------------------------
		bool			HasNewSkill() const		{ return m_bNewSkill; }
		void			SetNewSkill()			{ m_bNewSkill = true; }	
		void			UnSetNewSkill()			{ m_bNewSkill = false; }
		
		//------------------------------------------------------
		// Learn Skill
		//------------------------------------------------------
		bool			LearnSkill(ACTIONINFO id);		
		bool			UnLearnSkill(ACTIONINFO id);	

		//------------------------------------------------------
		// Iterator
		//------------------------------------------------------
		void			SetBegin()			{ m_iterator = m_mapSkillID.begin(); }
		ACTIONINFO		GetSkillID()		{ return (*m_iterator).first; }
		SKILLSTATUS		GetSkillStatus()	{ return (*m_iterator).second; }
		void			Next()				{ m_iterator++; }		
		bool			IsEnd() const		{ return m_iterator==m_mapSkillID.end(); }
		bool			IsNotEnd() const	{ return m_iterator!=m_mapSkillID.end(); }
		int				GetSize() const		{ return m_mapSkillID.size(); }

		//------------------------------------------------------
		// Level / Exp
		//------------------------------------------------------
		int			GetDomainLevel() const		{ return m_DomainLevel; }
		void		SetDomainLevel(int dl)		{ m_DomainLevel = dl; }
		DWORD		GetDomainExpRemain() const		{ return m_DomainExpRemain; }
		void		SetDomainExpRemain(DWORD de)		{ m_DomainExpRemain = de; }

		//------------------------------------------------------
		// Skill Step 
		//------------------------------------------------------
		BOOL								IsExistSkillStep(SKILL_STEP ss) const;
		const SKILL_STEP_LIST*				GetSkillStepList(SKILL_STEP ss) const;		
		

		//------------------------------------------------------
		// File I/O
		//------------------------------------------------------
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);

		//------------------------------------------------------
		// Load SERVER_SKILLINFO
		//------------------------------------------------------
		void		LoadFromFileServerDomainInfo(std::ifstream& file);

		//------------------------------------------------------
		// Exp
		//------------------------------------------------------
		const ExpInfo&	GetExpInfo(int level) const;

		
		void		AddNextSkillForce(ACTIONINFO id);
		bool		IsAvailableDeleteSkill(ACTIONINFO id);
		// 2004, 11, 9, sobeit add end
	protected :	

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		void		RemoveNextSkill(ACTIONINFO id);
		void		AddNextSkill(ACTIONINFO id);

		//------------------------------------------------------
		// Level
		//------------------------------------------------------
		void		SetMaxLevel();

		//------------------------------------------------------
		// Add Skill Step
		//------------------------------------------------------
		
		void		AddSkillStep(SKILL_STEP ss, ACTIONINFO ai);


	protected :
		SKILLID_MAP::const_iterator	m_iterator;
		SKILLID_MAP					m_mapSkillID;		
		
		int							m_MaxLevel;			
		int							m_MaxLearnedLevel;	
		ACTIONINFO*					m_pLearnedSkillID;	

		
		bool				 		m_bNewSkill;

		// Domain Level
		int							m_DomainLevel;
		DWORD						m_DomainExpRemain;

		// SkillStep map
		SKILL_STEP_MAP				m_mapSkillStep;

		//
		ExpTable					m_DomainExpTable;
};


//----------------------------------------------------------------------
//
//	SkillInfoTable
//
//----------------------------------------------------------------------
class MSkillInfoTable : public CTypeTable<SKILLINFO_NODE> {
	public :
		MSkillInfoTable();
		~MSkillInfoTable();

		
		void			Init();

		void			LoadFromFileServerSkillInfo(std::ifstream& file);
		void			SaveFromFileServerSkillInfo(std::ofstream& file);
};

extern MSkillInfoTable*		g_pSkillInfoTable;


//----------------------------------------------------------------------
//
// SkillManager
//
//----------------------------------------------------------------------
class MSkillManager : public CTypeTable<MSkillDomain>
{
	public :
		MSkillManager();
		~MSkillManager();

		void			Init();
		void			InitSkillList();

		//------------------------------------------------------
		// Load SERVER_SKILLINFO
		//------------------------------------------------------
		void		LoadFromFileServerDomainInfo(std::ifstream& file);
};

//----------------------------------------------------------------------
//
//	global
//
//----------------------------------------------------------------------
extern MSkillManager*	g_pSkillManager;

extern MSkillSet*		g_pSkillAvailable;

#define					HOLYLAND_BONUS_MAX	12
#define					SWEEPER_BONUS_MAX	12

extern bool				g_abHolyLandBonusSkills[HOLYLAND_BONUS_MAX];
extern bool				g_abSweeperBonusSkills[SWEEPER_BONUS_MAX];
#endif

