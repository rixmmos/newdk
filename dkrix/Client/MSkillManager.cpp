//----------------------------------------------------------------------
// MSkillManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MSkillManager.h"
#include "MTypeDef.h"

#ifdef __GAME_CLIENT__
	#include "MPlayer.h"
	#include "MSlayerGear.h"
	#include "MInventory.h"
	#include "ServerInfo.h"
	#include "MHelpManager.h"
//	#include "MFileDef.h"
	#include "Packet/Properties.h"
	#include "MItemFinder.h"
	#include "UserInformation.h"
	#include "VS_UI.h"		
	#include "MZone.h"		
#include "DebugInfo.h"

	extern MItem* UI_GetMouseItem();
	extern bool IsBombMaterial(const MItem* pItem);

	extern DWORD	g_CurrentTime;
#endif

#ifdef __GAME_CLIENT__
	bool	FileOpenBinary(const char* filename, std::ifstream& file);

	
	bool	IsBombMaterial(const MItem* pItem);

	bool	g_abHolyLandBonusSkills[12] = { false, };
	bool	g_abSweeperBonusSkills[SWEEPER_BONUS_MAX] = { false, };

#endif

//----------------------------------------------------------------------
//
// Global
//
//----------------------------------------------------------------------
// SkillInfoTable
MSkillInfoTable	*					g_pSkillInfoTable = NULL;

// SkillManager
MSkillManager*						g_pSkillManager = NULL;

// SkillAvailable
MSkillSet*							g_pSkillAvailable = NULL;

// 
//----------------------------------------------------------------------
//
//							SKILLINFO_NODE
//
//----------------------------------------------------------------------
SKILLINFO_NODE::SKILLINFO_NODE()
{
	m_Level = 1;

	m_SpriteID = 0;

	m_MP = 0;
		
	m_bPassive = false;
	m_bActive = false;
		
	m_ExpLevel = 0;			
	m_SkillExp = 0;			

	m_LearnLevel = 100;
	m_eSkillRace = RACE_SLAYER;

	m_DelayTime = 0;		
	m_AvailableTime = 0;	
		
	m_bEnable = false;	

	DomainType = 0;		
	minDamage = 0;		
	maxDamage = 0;		
	minDelay = 0;		
	maxDelay = 0;		
	minCastTime = 0;	
	maxCastTime = 0;	
	minDuration = 0;	
	maxDuration = 0;	
	minRange = 1;		
	maxRange = 1;		
	maxExp = 0;			
	SkillPoint = 0;
	LevelUpPoint = 0;
	Fire= 0;
	Water= 0;
	Earth= 0;
	Wind= 0;
	Sum= 0;
	Wristlet= 0;
	Stone1= 0;
	Stone2= 0;
	Stone3= 0;
	Stone4= 0;
	ElementalDomain= 0;
	CanDelete = 0;
}
//----------------------------------------------------------------------
// Save From File ServerSkillInfo
//----------------------------------------------------------------------
void 
SKILLINFO_NODE::SaveFromFileServerSkillInfo(ofstream &file)
{
	
	file.write((char*)&m_LearnLevel, 4);
	file.write((char*)&DomainType, 4);
	m_Name.SaveToFile( file );
	m_HName.SaveToFile( file );
	file.write((char*)&minDamage, 4);		
	file.write((char*)&maxDamage, 4);		
	file.write((char*)&minDelay, 4);			
	file.write((char*)&maxDelay, 4);			
	file.write((char*)&minDuration, 4);		
	file.write((char*)&maxDuration, 4);		
	file.write((char*)&m_MP, 4);					
	file.write((char*)&minRange, 4);			
	file.write((char*)&maxRange, 4);			
	file.write((char*)&maxExp, 4);			
	
	if(DomainType == SKILLDOMAIN_OUSTERS)
	{
		file.write((char*)&SkillPoint,		sizeof(int));
		file.write((char*)&LevelUpPoint,		sizeof(int));
		int szSkill= SkillTypeList.size();
		file.write((char*)&szSkill,			sizeof(int));
		SKILLTYPE_LIST::const_iterator iSkillID = SkillTypeList.begin();
		while (iSkillID!=SkillTypeList.end())
 		{
 			int skillType = *iSkillID;
 
 			file.write((const char*)&skillType, sizeof(int));
 
 			iSkillID++;
 		}
// 		for(int i = 0; i < szSkill; i++)
// 		{
// 			int skillType;
// 			file.write((char*)&skillType,		sizeof(int));
// 			SkillTypeList.push_back(skillType);
// 		}
		file.write((char*)&Fire,				sizeof(int));
		file.write((char*)&Water,			sizeof(int));
		file.write((char*)&Earth,			sizeof(int));
		file.write((char*)&Wind,				sizeof(int));
		file.write((char*)&Sum,				sizeof(int));
		file.write((char*)&Wristlet,			sizeof(int));
		file.write((char*)&Stone1,			sizeof(int));
		file.write((char*)&Stone2,			sizeof(int));
		file.write((char*)&Stone3,			sizeof(int));
		file.write((char*)&Stone4,			sizeof(int));
		file.write((char*)&ElementalDomain,	sizeof(int));
		file.write((char*)&CanDelete,		sizeof(BYTE));
	} 
}
//----------------------------------------------------------------------
// Load From File ServerSkillInfo
//----------------------------------------------------------------------
void		
SKILLINFO_NODE::LoadFromFileServerSkillInfo(std::ifstream& file)
{
	int ll;
	MString name;
	MString hname;
	int mp;
	
	file.read((char*)&ll, 4);
	file.read((char*)&DomainType, 4);
	name.LoadFromFile( file );
	hname.LoadFromFile( file );
	file.read((char*)&minDamage, 4);		
	file.read((char*)&maxDamage, 4);		
	file.read((char*)&minDelay, 4);			
	file.read((char*)&maxDelay, 4);			
	file.read((char*)&minDuration, 4);		
	file.read((char*)&maxDuration, 4);		
	file.read((char*)&mp, 4);					
	file.read((char*)&minRange, 4);			
	file.read((char*)&maxRange, 4);			
	file.read((char*)&maxExp, 4);			
	
	if(DomainType == SKILLDOMAIN_OUSTERS)
	{
		file.read((char*)&SkillPoint,		sizeof(int));
		file.read((char*)&LevelUpPoint,		sizeof(int));
		int szSkill;
		file.read((char*)&szSkill,			sizeof(int));
		SkillTypeList.clear();
		for(int i = 0; i < szSkill; i++)
		{
			int skillType;
			file.read((char*)&skillType,		sizeof(int));
			SkillTypeList.push_back(skillType);
		}
		file.read((char*)&Fire,				sizeof(int));
		file.read((char*)&Water,			sizeof(int));
		file.read((char*)&Earth,			sizeof(int));
		file.read((char*)&Wind,				sizeof(int));
		file.read((char*)&Sum,				sizeof(int));
		file.read((char*)&Wristlet,			sizeof(int));
		file.read((char*)&Stone1,			sizeof(int));
		file.read((char*)&Stone2,			sizeof(int));
		file.read((char*)&Stone3,			sizeof(int));
		file.read((char*)&Stone4,			sizeof(int));
		file.read((char*)&ElementalDomain,	sizeof(int));
		file.read((char*)&CanDelete,		sizeof(BYTE));
	} else
	{
		ElementalDomain = -1;
	}
	if (name != "Empty Skill")
	{
		m_LearnLevel = ll;
		m_Name = name;
		m_HName = hname;
		m_MP = mp;
	}
}

//----------------------------------------------------------------------
// Add NextSkill
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
SKILLINFO_NODE::AddNextSkill(ACTIONINFO id)
{
	SKILLID_LIST::iterator iSkill = m_listNextSkill.begin();

	
	while (iSkill != m_listNextSkill.end())
	{
		
		if (*iSkill==id)
		{
			return false;
		}
		
		else if (*iSkill > id)
		{
			
			m_listNextSkill.insert( iSkill, id );

			return true;
		}

		iSkill++;
	}

	m_listNextSkill.push_back( id );

	return true;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void		
SKILLINFO_NODE::SaveToFile(std::ofstream& file)
{

	m_Name.SaveToFile( file );							
	m_HName.SaveToFile( file );
	file.write((const char*)&m_Level, 4);
	file.write((const char*)&m_X, 4);
	file.write((const char*)&m_Y, 4);					
	file.write((const char*)&m_SpriteID, SIZE_SPRITEID);	
	file.write((const char*)&m_MP, 4);				
	file.write((const char*)&m_bPassive, 1);		
	file.write((const char*)&m_bActive, 1);			

	BYTE skillStep = m_SkillStep;
	file.write((const char*)&skillStep, 1);

	
	int idNum = m_listNextSkill.size();
	file.write((const char*)&idNum, 4);
	SKILLID_LIST::const_iterator iSkillID = m_listNextSkill.begin();

	while (iSkillID!=m_listNextSkill.end())
	{
		TYPE_ACTIONINFO id = *iSkillID;

		file.write((const char*)&id, SIZE_ACTIONINFO);

		iSkillID++;
	}
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void		
SKILLINFO_NODE::LoadFromFile(std::ifstream& file)
{
	m_Name.LoadFromFile( file );					
	m_HName.LoadFromFile( file );
	file.read((char*)&m_Level, 4);
	file.read((char*)&m_X, 4);
	file.read((char*)&m_Y, 4);						
	file.read((char*)&m_SpriteID, SIZE_SPRITEID);	
	file.read((char*)&m_MP, 4);						
	file.read((char*)&m_bPassive, 1);				
	file.read((char*)&m_bActive, 1);				

	BYTE skillStep;
	file.read((char*)&skillStep, 1);
	m_SkillStep = (SKILL_STEP)skillStep;
	// id list load
	int idNum;
	file.read((char*)&idNum, 4);

	m_listNextSkill.clear();
	for (int i=0; i<idNum; i++)
	{
		TYPE_ACTIONINFO id;

		file.read((char*)&id, SIZE_ACTIONINFO);

		m_listNextSkill.push_back( (ACTIONINFO)id );
	}	

	
	m_ExpLevel = 0;
}	

//----------------------------------------------------------------------
// Set DelayTime ( delay )
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
SKILLINFO_NODE::SetDelayTime(DWORD delay)		
{
	
	if (delay < 1800)
	{
		delay = 0;
	}			
				
	m_DelayTime = delay;	
}

//----------------------------------------------------------------------
// Is AvailableTime ?
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
SKILLINFO_NODE::IsAvailableTime() const		
{
	#if defined(__GAME_CLIENT__)
		return g_CurrentTime >= m_AvailableTime;
	#else
		return true;
	#endif
}

//----------------------------------------------------------------------
// Get AvailableTimeLeft
//----------------------------------------------------------------------

//----------------------------------------------------------------------
DWORD				
SKILLINFO_NODE::GetAvailableTimeLeft() const
{
	#if defined(__GAME_CLIENT__)
		int timeGap = (int)m_AvailableTime - (int)g_CurrentTime;

		if (timeGap > 0)
		{
			return timeGap;
		}
	#endif

	return 0;
}

//----------------------------------------------------------------------
// Set AvailableTime
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
SKILLINFO_NODE::SetAvailableTime(int delay)
{
	
	#ifdef __GAME_CLIENT__
	if(delay == 0)
		m_AvailableTime = 0;	//g_CurrentTime;
	else
		m_AvailableTime = g_CurrentTime+delay;	//g_CurrentTime;
	#endif
}

//----------------------------------------------------------------------
// Set Next AvailableTime
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
SKILLINFO_NODE::SetNextAvailableTime()
{
	#ifdef __GAME_CLIENT__
		
		m_AvailableTime = g_CurrentTime + m_DelayTime;
	#endif
}

//----------------------------------------------------------------------
// Set Enable
//----------------------------------------------------------------------
void				
SKILLINFO_NODE::SetEnable(bool enable)
{
	m_bEnable = enable;
}

//----------------------------------------------------------------------
// Set Disable
//----------------------------------------------------------------------
/*
void				
SKILLINFO_NODE::SetDisable()
{
	m_bEnable = false;
}
*/

//----------------------------------------------------------------------
//
//							MSkillSet
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Add Skill
//----------------------------------------------------------------------
bool			
MSkillSet::AddSkill(ACTIONINFO id, BYTE flag)
{
	SKILLID_MAP::iterator	iSkill;

	iSkill = find( id );
	
	//-----------------------------------------------
	
	//-----------------------------------------------
	if (iSkill == end())
	{
		insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));

		return true;
	}
	
	//-----------------------------------------------
	
	//-----------------------------------------------
	SKILLID_NODE& node = (*iSkill).second;
	
	node.Flag = flag;

	return false;
}

//----------------------------------------------------------------------
// Remove Skill
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MSkillSet::RemoveSkill(ACTIONINFO id)
{
	SKILLID_MAP::iterator	iSkill;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iSkill = find(id);
    
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iSkill == end())
	{
		return false;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	
	erase( iSkill );

	return true;
}

//----------------------------------------------------------------------
// Is Enable Skill?
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MSkillSet::IsEnableSkill(ACTIONINFO id) const
{
	SKILLID_MAP::const_iterator		iSkill;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iSkill = find(id);
    
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iSkill == end())
	{
		return false;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
			
	return ((*iSkill).second).IsEnable()!=0;	
}

//----------------------------------------------------------------------
// Enable Skill
//----------------------------------------------------------------------
bool			
MSkillSet::EnableSkill(ACTIONINFO id)
{
	SKILLID_MAP::iterator	iSkill;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iSkill = find(id);
    
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iSkill == end())
	{
		return false;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	
	((*iSkill).second).SetEnable();

	return true;
}

//----------------------------------------------------------------------
// Disable Skill 
//----------------------------------------------------------------------
bool			
MSkillSet::DisableSkill(ACTIONINFO id)	
{
	SKILLID_MAP::iterator	iSkill;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iSkill = find(id);
    
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iSkill == end())
	{
		return false;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	
	((*iSkill).second).SetDisable();

	return true;
}

//----------------------------------------------------------------------
// Set Avaliable Skills
//----------------------------------------------------------------------

//




//----------------------------------------------------------------------
void
MSkillSet::SetAvailableSkills()
{
#ifdef __GAME_CLIENT__

	if (g_pPlayer==NULL 		
		|| g_pSkillManager==NULL
		|| g_pSkillManager->GetSize()==0		
		|| g_pSkillInfoTable==NULL
		|| g_pSkillInfoTable->GetSize()==0)
	{
		return;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	int playerMP;		
	BYTE flag;
	
	if (g_pPlayer->GetRace() != RACE_VAMPIRE)
	{
		playerMP = g_pPlayer->GetMP();	

		
		if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SACRIFICE))
		{
			playerMP += (g_pPlayer->GetHP() << 1);
		}		
	}
	else
	{
		
		playerMP = g_pPlayer->GetHP();	
	}

	
	clear();
	
	if( g_pZone != NULL && g_pZone->GetID() == 3001 )
		return;


	//-----------------------------------------------------
	//
	
	//
	//-----------------------------------------------------
	switch(g_pPlayer->GetRace())
	{
		case RACE_SLAYER:
		{
			if (g_pSlayerGear==NULL
				|| g_pInventory==NULL)
			{
				return;
			}
			
			if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
			{
				insert(SKILLID_MAP::value_type( MAGIC_UN_TRANSFORM, SKILLID_NODE(MAGIC_UN_TRANSFORM, FLAG_SKILL_ENABLE) ));
				if ((*g_pSkillInfoTable)[SKILL_TURRET_FIRE].GetMP() > playerMP)
					insert(SKILLID_MAP::value_type( SKILL_TURRET_FIRE, SKILLID_NODE(SKILL_TURRET_FIRE, 0) ));
				else
					insert(SKILLID_MAP::value_type( SKILL_TURRET_FIRE, SKILLID_NODE(SKILL_TURRET_FIRE, FLAG_SKILL_ENABLE) ));
				insert(SKILLID_MAP::value_type( SKILL_VIVID_MAGAZINE, SKILLID_NODE(SKILL_VIVID_MAGAZINE, FLAG_SKILL_ENABLE) ));
				return;
			}
			
			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------
			BYTE fDomain[MAX_SKILLDOMAIN];
			
			
			const MItem* pItem = (*g_pSlayerGear).GetItem( (MSlayerGear::GEAR_SLAYER)MSlayerGear::GEAR_SLAYER_RIGHTHAND );

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			fDomain[SKILLDOMAIN_GUN]	= 0;
			fDomain[SKILLDOMAIN_BLADE]	= 0;
			fDomain[SKILLDOMAIN_SWORD]	= 0;

			if (pItem!=NULL && pItem->IsAffectStatus())
			{	
				//-----------------------------------------------------
				
				//-----------------------------------------------------
				if (pItem->IsGunItem())
				{
					fDomain[SKILLDOMAIN_GUN]	= FLAG_SKILL_ENABLE;
				}
				//-----------------------------------------------------
				
				//-----------------------------------------------------
				else if (pItem->GetItemClass()==ITEM_CLASS_SWORD)
				{
					fDomain[SKILLDOMAIN_SWORD]	= FLAG_SKILL_ENABLE;
				}
				//-----------------------------------------------------
				
				//-----------------------------------------------------
				else if (pItem->GetItemClass()==ITEM_CLASS_BLADE)
				{
					fDomain[SKILLDOMAIN_BLADE]	= FLAG_SKILL_ENABLE;
				}
			}
			
			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------
			//-----------------------------------------------------
			// Blade
			//-----------------------------------------------------
			MSkillDomain& bladeDomain = (*g_pSkillManager)[SKILLDOMAIN_BLADE];

			bladeDomain.SetBegin();		
			while (bladeDomain.IsNotEnd())
			{
				MSkillDomain::SKILLSTATUS	status	= bladeDomain.GetSkillStatus();

				
				if (status == MSkillDomain::SKILLSTATUS_LEARNED)
				{
					ACTIONINFO id = bladeDomain.GetSkillID();
		
					if ((*g_pSkillInfoTable)[id].GetMP() > playerMP)
					{
						flag = 0;
					}
					else
					{
						flag = fDomain[SKILLDOMAIN_BLADE];

						if ((*g_pSkillInfoTable)[id].IsActive())
						{
							flag |= FLAG_SKILL_ENABLE;
						}					
					}

					insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
				}

				
				bladeDomain.Next();
			}

			//-----------------------------------------------------
			// Sword
			//-----------------------------------------------------
			MSkillDomain& swordDomain = (*g_pSkillManager)[SKILLDOMAIN_SWORD];

			swordDomain.SetBegin();		
			while (swordDomain.IsNotEnd())
			{
				MSkillDomain::SKILLSTATUS	status	= swordDomain.GetSkillStatus();

				
				if (status == MSkillDomain::SKILLSTATUS_LEARNED)
				{
					ACTIONINFO id = swordDomain.GetSkillID();

					if ((*g_pSkillInfoTable)[id].GetMP() > playerMP)
					{
						flag = 0;
					}
					else
					{
						flag = fDomain[SKILLDOMAIN_SWORD];

						if ((*g_pSkillInfoTable)[id].IsActive())
						{
							flag |= FLAG_SKILL_ENABLE;
						}					
					}

					insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
				}

				
				swordDomain.Next();
			}

			//-----------------------------------------------------
			// Gun
			//-----------------------------------------------------
			MSkillDomain& gunDomain = (*g_pSkillManager)[SKILLDOMAIN_GUN];

			gunDomain.SetBegin();		
			while (gunDomain.IsNotEnd())
			{
				MSkillDomain::SKILLSTATUS	status	= gunDomain.GetSkillStatus();
				
				
				if (status == MSkillDomain::SKILLSTATUS_LEARNED)
				{
					ACTIONINFO id = gunDomain.GetSkillID();

					if ((*g_pSkillInfoTable)[id].GetMP() > playerMP)
					{
						flag = 0;
					}
					else
					{
						flag = fDomain[SKILLDOMAIN_GUN];

						if ((*g_pSkillInfoTable)[id].IsActive())
						{
							flag |= FLAG_SKILL_ENABLE;
						}					
					}

					insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
				}

				
				gunDomain.Next();
			}

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			MSkillDomain& enchantDomain = (*g_pSkillManager)[SKILLDOMAIN_ENCHANT];

			enchantDomain.SetBegin();		
			while (enchantDomain.IsNotEnd())
			{
				MSkillDomain::SKILLSTATUS	status	= enchantDomain.GetSkillStatus();

				
				if (status == MSkillDomain::SKILLSTATUS_LEARNED)
				{
					ACTIONINFO id = enchantDomain.GetSkillID();

					if ((*g_pSkillInfoTable)[id].GetMP() > playerMP)
					{
						flag = 0;
					}
					else
					{
						flag = FLAG_SKILL_ENABLE;
					}

					insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
				}

				
				enchantDomain.Next();
			}

			
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			MSkillDomain& healDomain = (*g_pSkillManager)[SKILLDOMAIN_HEAL];

			healDomain.SetBegin();		
			while (healDomain.IsNotEnd())
			{
				MSkillDomain::SKILLSTATUS	status	= healDomain.GetSkillStatus();

				
				if (status == MSkillDomain::SKILLSTATUS_LEARNED)
				{
					ACTIONINFO id = healDomain.GetSkillID();

					if ((*g_pSkillInfoTable)[id].GetMP() > playerMP)
					{
						flag = 0;
					}
					else
					{
						flag = FLAG_SKILL_ENABLE;
					}

					insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
				}

				
				healDomain.Next();
			}

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			MSkillDomain& etcDomain = (*g_pSkillManager)[SKILLDOMAIN_ETC];

			etcDomain.SetBegin();		
			while (etcDomain.IsNotEnd())
			{
				MSkillDomain::SKILLSTATUS	status	= etcDomain.GetSkillStatus();

				
				if (status == MSkillDomain::SKILLSTATUS_LEARNED)
				{
					ACTIONINFO id = etcDomain.GetSkillID();

					if ((*g_pSkillInfoTable)[id].GetMP() > playerMP)
					{
						flag = 0;
					}
					else
					{
						flag = FLAG_SKILL_ENABLE;
					}

					erase(id);
					insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
				}

				
				etcDomain.Next();
			}

			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------
			BOOL bCheckHolyWater	= TRUE;
			BOOL bCheckPortal		= TRUE;

			BOOL bCheckInstallMine = (gunDomain.GetSkillStatus(SKILL_INSTALL_MINE)==MSkillDomain::SKILLSTATUS_LEARNED);
			BOOL bCheckCreateMine = (gunDomain.GetSkillStatus(SKILL_MAKE_MINE)==MSkillDomain::SKILLSTATUS_LEARNED);
			BOOL bCheckCreateBomb = (gunDomain.GetSkillStatus(SKILL_MAKE_BOMB)==MSkillDomain::SKILLSTATUS_LEARNED);		
			
			BOOL bCheckBomb			= (gunDomain.GetSkillStatus(SKILL_THROW_BOMB)==MSkillDomain::SKILLSTATUS_LEARNED);
			BOOL bCheckBombOrMine   = bCheckBomb || bCheckInstallMine;
			BOOL bCheckBombOrMineMaterial   = bCheckCreateBomb || bCheckCreateMine;
			
			BOOL bHasBomb			= FALSE;
			BOOL bHasMine			= FALSE;
			BOOL bHasMineMaterial	= FALSE;
			BOOL bHasBombMaterial	= FALSE;

			g_pInventory->SetBegin();

			while (g_pInventory->IsNotEnd())
			{
				const MItem* pItem = g_pInventory->Get();

				ITEM_CLASS itemClass = pItem->GetItemClass();

			#ifdef __TEST_SUB_INVENTORY__   
				if(itemClass == ITEM_CLASS_SUB_INVENTORY)
				{
					MSubInventory* pSubItem = (MSubInventory*)pItem;
					pSubItem->SetBegin();
					while(pSubItem->IsNotEnd())
					{

						const MItem* pSubSbuItem = pSubItem->Get();
						if(NULL != pSubSbuItem && bCheckPortal && pSubSbuItem->GetItemClass() == ITEM_CLASS_SLAYER_PORTAL_ITEM)
						{
							flag = FLAG_SKILL_ENABLE;
							insert(SKILLID_MAP::value_type( SUMMON_HELICOPTER, SKILLID_NODE(SUMMON_HELICOPTER, flag)) );
							bCheckPortal = FALSE;
						}
						pSubItem->Next();
					}
				}
			#endif

				//-----------------------------------------------------
				// Portal
				//-----------------------------------------------------
				if (bCheckPortal && itemClass==ITEM_CLASS_SLAYER_PORTAL_ITEM)
				{				
					flag = FLAG_SKILL_ENABLE;
				
					insert(SKILLID_MAP::value_type( SUMMON_HELICOPTER, SKILLID_NODE(SUMMON_HELICOPTER, flag)) );

					bCheckPortal = FALSE;
				}
				//-----------------------------------------------------
				// HolyWater
				//-----------------------------------------------------			
				else if (bCheckHolyWater && itemClass==ITEM_CLASS_HOLYWATER)
				{
					if ((*g_pSkillInfoTable)[MAGIC_THROW_HOLY_WATER].GetMP() > playerMP)
					{
						flag = 0;
					}
					else
					{
						flag = FLAG_SKILL_ENABLE;
					}

					insert(SKILLID_MAP::value_type( MAGIC_THROW_HOLY_WATER, SKILLID_NODE(MAGIC_THROW_HOLY_WATER, flag)) );				

					
//					__BEGIN_HELP_EVENT
//						ExecuteHelpEvent( HE_ITEM_APPEAR_HOLY_WATER );	
//					__END_HELP_EVENT

					bCheckHolyWater = FALSE;
				}
				//-----------------------------------------------------
				
				//-----------------------------------------------------
				else if (bCheckBombOrMine
						&& (itemClass==ITEM_CLASS_BOMB
							|| itemClass==ITEM_CLASS_MINE))
				{
					if (itemClass==ITEM_CLASS_BOMB)
					{
						bHasBomb = TRUE;

						if (bCheckBomb)	flag = FLAG_SKILL_ENABLE;
									else flag = 0;
					}
					else
					{
						bHasMine = TRUE;

						if (bCheckInstallMine) flag = FLAG_SKILL_ENABLE;
										else flag = 0;					
					}

					int skillID = pItem->GetUseActionInfo();

					

					
					if (find((ACTIONINFO)skillID)==end())
					{
						insert(SKILLID_MAP::value_type( (ACTIONINFO)skillID, SKILLID_NODE((ACTIONINFO)skillID, flag)) );
					}
				}
				//-----------------------------------------------------
				
				//-----------------------------------------------------
				else if (bCheckBombOrMineMaterial
							&& itemClass==ITEM_CLASS_BOMB_MATERIAL)
				{
					if (IsBombMaterial(pItem))
					{
						bHasBombMaterial = TRUE;
					}
					else
					{
						bHasMineMaterial = TRUE;
					}
				}
				
				
				g_pInventory->Next();
			}

			
			MItem* pMouseItem = UI_GetMouseItem();

			if (pMouseItem!=NULL)
			{
				ITEM_CLASS	itemClass	= pMouseItem->GetItemClass();
				bool isBombMaterial = IsBombMaterial(pMouseItem);

				bHasBomb			= bHasBomb || itemClass==ITEM_CLASS_BOMB;
				bHasMine			= bHasMine || itemClass==ITEM_CLASS_MINE;

				bHasMineMaterial	= bHasMineMaterial || itemClass==ITEM_CLASS_BOMB_MATERIAL && !isBombMaterial;
				bHasBombMaterial	= bHasBombMaterial || itemClass==ITEM_CLASS_BOMB_MATERIAL && isBombMaterial;
			}

			
			if (bCheckInstallMine)
			{
				ACTIONINFO skillID = SKILL_INSTALL_MINE;
				flag = (IsEnableSkill(skillID) && bHasMine? FLAG_SKILL_ENABLE : 0);
				
				iterator iSkill = find( skillID );
				if (iSkill != end())
				{			
					iSkill->second.Flag = flag;
				}
				else
				{
					insert(SKILLID_MAP::value_type( skillID, SKILLID_NODE(skillID, flag)) );
				}
			}

			
			if (bCheckCreateMine)
			{
				ACTIONINFO skillID = SKILL_MAKE_MINE;
				flag = (IsEnableSkill(skillID) && bHasMineMaterial? FLAG_SKILL_ENABLE : 0);
				
				iterator iSkill = find( skillID );
				if (iSkill != end())
				{			
					iSkill->second.Flag = flag;
				}
				else
				{
					insert(SKILLID_MAP::value_type( skillID, SKILLID_NODE(skillID, flag)) );
				}
			}

			
			if (bCheckCreateBomb)
			{
				ACTIONINFO skillID = SKILL_MAKE_BOMB;
				flag = (IsEnableSkill(skillID) && bHasBombMaterial? FLAG_SKILL_ENABLE : 0);
				
				iterator iSkill = find( skillID );
				if (iSkill != end())
				{			
					iSkill->second.Flag = flag;
				}
				else
				{
					insert(SKILLID_MAP::value_type( skillID, SKILLID_NODE(skillID, flag)) );
				}
			}

			
			if (bCheckBomb)
			{
				ACTIONINFO skillID = SKILL_THROW_BOMB;
				flag = (IsEnableSkill(skillID) && bHasBomb? FLAG_SKILL_ENABLE : 0);
				
				iterator iSkill = find( skillID );
				if (iSkill != end())
				{			
					iSkill->second.Flag = flag;
				}
				else
				{
					insert(SKILLID_MAP::value_type( skillID, SKILLID_NODE(skillID, flag)) );
				}
			}

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (g_pUserInformation->HasSkillRestore)
			{
				if ((*g_pSkillInfoTable)[MAGIC_RESTORE].GetMP() > playerMP)
				{
					flag = 0;
				}
				else
				{
					flag = FLAG_SKILL_ENABLE;
				}

				insert(SKILLID_MAP::value_type( MAGIC_RESTORE, SKILLID_NODE(MAGIC_RESTORE, flag) ));
			}
		}
		break;

	//-----------------------------------------------------
	//
	
	//
	//-----------------------------------------------------
	case RACE_VAMPIRE:
		{		
			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------
			
			if (g_pPlayer->IsInCasket())
			{
				flag = FLAG_SKILL_ENABLE;

				insert(SKILLID_MAP::value_type( MAGIC_OPEN_CASKET, SKILLID_NODE(MAGIC_OPEN_CASKET, flag) ));
				gC_vs_ui.SelectSkill( MAGIC_OPEN_CASKET );

				return;
			}
			
			
			if (g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE1
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_FEMALE1
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE2
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_FEMALE2
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE3
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_FEMALE3
				// add by Coffee 2006.12.7
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE4
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_FEMALE4
				//add by viva
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE5
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_FEMALE5
				//add by viva
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE6
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_FEMALE6
				// end
				|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_OPERATOR)

			{
				MSkillDomain& vampireDomain = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE];
				
				vampireDomain.SetBegin();		

			#ifdef __TEST_SUB_INVENTORY__   
				MItem* pSubInventory = NULL;
			#endif

				while (vampireDomain.IsNotEnd())
				{
					MSkillDomain::SKILLSTATUS	status	= vampireDomain.GetSkillStatus();

					
					if (status == MSkillDomain::SKILLSTATUS_LEARNED)
					{
						ACTIONINFO id = vampireDomain.GetSkillID();

						
						if ((*g_pSkillInfoTable)[id].GetMP() > playerMP
							|| id == MAGIC_INVISIBILITY && (g_pZone->GetID() == 1104 || g_pZone->GetID() == 1106 || g_pZone->GetID() == 1114 || g_pZone->GetID() == 1115))
						{
							flag = 0;
						}
						else
						{
							flag = FLAG_SKILL_ENABLE;					
						}

						
						switch (id)
						{
							case MAGIC_BLOODY_MARK :
							{
								MVampirePortalItemFinder finder(false);
							#ifdef __TEST_SUB_INVENTORY__   
								if (NULL == ((MItemManager*)g_pInventory)->FindItemAll( finder , pSubInventory))
							#else
								if (NULL == ((MItemManager*)g_pInventory)->FindItem( finder ))
							#endif
								{
									flag = 0;
								}
							}
							break;

							case MAGIC_BLOODY_TUNNEL :
							{
								MVampirePortalItemFinder finder(true);
							#ifdef __TEST_SUB_INVENTORY__   
								if (NULL == ((MItemManager*)g_pInventory)->FindItemAll( finder , pSubInventory ))
							#else
								if (NULL == ((MItemManager*)g_pInventory)->FindItem( finder ))
							#endif
								{
									flag = 0;
								}
							}
								{
									flag = 0;
								}
							break;

							case MAGIC_TRANSFORM_TO_WOLF :
								if (NULL == g_pInventory->FindItem( ITEM_CLASS_VAMPIRE_ETC, 0 ))
								{
									flag = 0;
								}							
							break;

							case MAGIC_TRANSFORM_TO_BAT :

							#ifdef __TEST_SUB_INVENTORY__   
								if (NULL == g_pInventory->FindItemAll( MItemClassTypeFinder(ITEM_CLASS_VAMPIRE_ETC , 1), pSubInventory ))
							#else
								if (NULL == g_pInventory->FindItem( ITEM_CLASS_VAMPIRE_ETC, 1 ))
							#endif
								{
									flag = 0;
								}							
							break;
							
							case SKILL_TRANSFORM_TO_WERWOLF :
								if(g_pInventory->FindItem( ITEM_CLASS_SKULL, 39) == NULL)
								{
									flag = 0;
								}
							break;

							case MAGIC_HOWL :
								flag=0;
							break;
							
						}

						insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
					}

					
					vampireDomain.Next();
				}
			}

			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------
			 

			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------
			
			

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (g_pUserInformation->HasMagicGroundAttack)
			{
				if ((*g_pSkillInfoTable)[MAGIC_GROUND_ATTACK].GetMP() > playerMP)
				{
					flag = 0;
				}
				else
				{
					flag = FLAG_SKILL_ENABLE;
				}

				insert(SKILLID_MAP::value_type( MAGIC_GROUND_ATTACK, SKILLID_NODE(MAGIC_GROUND_ATTACK, flag) ));
			}
			
			SetAvailableVampireSkills();

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (g_pUserInformation->HasMagicBloodySnake)
			{
				if ((*g_pSkillInfoTable)[MAGIC_BLOODY_SNAKE].GetMP() > playerMP)
				{
					flag = 0;
				}
				else
				{
					flag = FLAG_SKILL_ENABLE;
				}

				insert(SKILLID_MAP::value_type( MAGIC_BLOODY_SNAKE, SKILLID_NODE(MAGIC_BLOODY_SNAKE, flag) ));
			}

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (g_pUserInformation->HasMagicBloodyWarp)
			{
				if ((*g_pSkillInfoTable)[MAGIC_BLOODY_WARP].GetMP() > playerMP)
				{
					flag = 0;
				}
				else
				{
					flag = FLAG_SKILL_ENABLE;
				}

				insert(SKILLID_MAP::value_type( MAGIC_BLOODY_WARP, SKILLID_NODE(MAGIC_BLOODY_WARP, flag) ));
			}
		}
		break;

	case RACE_OUSTERS:
		{		
			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------
			{
				MSkillDomain& oustersDomain = (*g_pSkillManager)[SKILLDOMAIN_OUSTERS];
				
				oustersDomain.SetBegin();		
				while (oustersDomain.IsNotEnd())
				{
					MSkillDomain::SKILLSTATUS	status	= oustersDomain.GetSkillStatus();

					
					if (status == MSkillDomain::SKILLSTATUS_LEARNED)
					{
						ACTIONINFO id = oustersDomain.GetSkillID();

						SKILLINFO_NODE sInfo = (*g_pSkillInfoTable)[id];
						
						flag = 0;

						if (sInfo.GetMP() <= playerMP)
						{
							if(sInfo.IsActive())
							{
								flag = FLAG_SKILL_ENABLE;					
							}
							else
							{
								
								const MItem* pItem = (*g_pOustersGear).GetItem( (MOustersGear::GEAR_OUSTERS)MOustersGear::GEAR_OUSTERS_RIGHTHAND );
								
								if(sInfo.ElementalDomain == SKILLINFO_NODE::ELEMENTAL_DOMAIN_NO_DOMAIN || sInfo.ElementalDomain == SKILLINFO_NODE::ELEMENTAL_DOMAIN_WIND
									|| sInfo.ElementalDomain == SKILLINFO_NODE::ELEMENTAL_DOMAIN_ETC ||
									sInfo.GetSkillStep() == SKILL_STEP_ETC)
								{
									flag = FLAG_SKILL_ENABLE;
								}
								else
								{
									if (pItem!=NULL && pItem->IsAffectStatus())
									{	
										const int itemClass = pItem->GetItemClass();
										
										if (itemClass == ITEM_CLASS_OUSTERS_CHAKRAM && sInfo.ElementalDomain == SKILLINFO_NODE::ELEMENTAL_DOMAIN_COMBAT || 
											itemClass == ITEM_CLASS_OUSTERS_WRISTLET &&
												(
													sInfo.ElementalDomain == SKILLINFO_NODE::ELEMENTAL_DOMAIN_FIRE && sInfo.Fire <= g_pPlayer->GetElementalFire() ||
													sInfo.ElementalDomain == SKILLINFO_NODE::ELEMENTAL_DOMAIN_WATER && sInfo.Water <= g_pPlayer->GetElementalWater() ||
													sInfo.ElementalDomain == SKILLINFO_NODE::ELEMENTAL_DOMAIN_EARTH && sInfo.Earth <= g_pPlayer->GetElementalEarth()
												)
											)
										{
											flag = FLAG_SKILL_ENABLE;
										}
									}
								}
							}
						}

					#ifdef __TEST_SUB_INVENTORY__   
						if( id == SKILL_SUMMON_SYLPH )
						{
							MItem* pSubInventory = NULL;
							if(NULL == ((MItemManager*)g_pInventory)->FindItemAll( MOustersSummonGemItemFinder(), pSubInventory ))
								flag = 0;
						}
					#else
						if( id == SKILL_SUMMON_SYLPH &&
							NULL == g_pInventory->FindItem( ITEM_CLASS_OUSTERS_SUMMON_ITEM ))
						{
							flag = 0;
						}
					#endif

						insert(SKILLID_MAP::value_type( id, SKILLID_NODE(id, flag) ));
					}

					
					oustersDomain.Next();
				}
			}

			//-----------------------------------------------------
			//
			
			//
			//-----------------------------------------------------

//			if (g_pPlayer->GetCreatureType()!=CREATURETYPE_BAT
//				&& g_pPlayer->GetCreatureType()!=CREATURETYPE_WOLF)
//			{
//				if ((*g_pSkillInfoTable)[SKILL_BLOOD_DRAIN].GetMP() > playerMP)
//				{
//					flag = 0;
//				}
//				else
//				{
//					flag = FLAG_SKILL_ENABLE;
//				}
//				insert(SKILLID_MAP::value_type( SKILL_BLOOD_DRAIN, SKILLID_NODE(SKILL_BLOOD_DRAIN, flag) ));
//			}

		}
		break;
	}





	//-----------------------------------------------------
	//
	
	//
	//-----------------------------------------------------
	int i;
	for(i = 0; i < HOLYLAND_BONUS_MAX; i++)
	{
		if(g_abHolyLandBonusSkills[i] == true)
		{
			insert(SKILLID_MAP::value_type( (ACTIONINFO)(SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA+i), SKILLID_NODE((ACTIONINFO)(SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA+i), FLAG_SKILL_ENABLE) ));
		}
	}

	for(i = 0; i < SWEEPER_BONUS_MAX; i++)
	{
		if( g_abSweeperBonusSkills[i] == true )
		{
			insert( SKILLID_MAP::value_type( (ACTIONINFO)(SKILL_SWEEPER_BONUS_1 + i), SKILLID_NODE( (ACTIONINFO)(SKILL_SWEEPER_BONUS_1+i), FLAG_SKILL_ENABLE) ) );
		}
	}

	if (g_pPlayer->GetCreatureType()!=CREATURETYPE_BAT
		&& g_pPlayer->GetCreatureType()!=CREATURETYPE_WOLF)
	{
		MPlayerGear* pGear;
		MItemClassFinder itemFinder( ITEM_CLASS_COUPLE_RING );
		
		switch(g_pPlayer->GetRace())
		{
		case RACE_SLAYER:
			pGear = g_pSlayerGear;							
			break;

		case RACE_VAMPIRE:
			pGear = g_pVampireGear;
			itemFinder.SetItemClass( ITEM_CLASS_VAMPIRE_COUPLE_RING);
			break;

		case RACE_OUSTERS:
			pGear = g_pOustersGear;
			break;
		}
		
		MItem *pItem = pGear->FindItem( itemFinder );
		if(pItem != NULL)
		{
			insert(SKILLID_MAP::value_type( (ACTIONINFO)SKILL_LOVE_CHAIN, SKILLID_NODE((ACTIONINFO)(SKILL_LOVE_CHAIN), pItem->IsAffectStatus() ) ));
		}
	}
	
//	insert(SKILLID_MAP::value_type( (ACTIONINFO)SKILL_MAGIC_ELUSION, SKILLID_NODE((ACTIONINFO)(SKILL_MAGIC_ELUSION),  FLAG_SKILL_ENABLE)) );

	g_pPlayer->CalculateLightSight();
	

	//CheckMP();

	
//	if( size() > 5 )
//		gC_vs_ui.ResetHotKey();
#endif
}

void			
MSkillSet::SetAvailableVampireSkills()
{
#ifdef __GAME_CLIENT__
	MSkillDomain& vampireDomain = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE];
	TYPE_CREATURETYPE PlayerCreatureType = g_pPlayer->GetCreatureType();

	int playerMP;		
	BYTE flag;
	
	if (g_pPlayer->GetRace() != RACE_VAMPIRE)
	{
		playerMP = g_pPlayer->GetMP();	

		
		if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SACRIFICE))
		{
			playerMP += (g_pPlayer->GetHP() << 1);
		}		
	}
	else
	{
		
		playerMP = g_pPlayer->GetHP();	
	}

	if (PlayerCreatureType!=CREATURETYPE_BAT
		&& PlayerCreatureType!=CREATURETYPE_WOLF
		&& PlayerCreatureType!=CREATURETYPE_WER_WOLF
		&& PlayerCreatureType!=CREATURETYPE_INSTALL_TURRET)
	{
		if ((*g_pSkillInfoTable)[SKILL_BLOOD_DRAIN].GetMP() > playerMP)
		{
			flag = 0;
		}
		else
		{
			flag = FLAG_SKILL_ENABLE;
		}
		insert(SKILLID_MAP::value_type( SKILL_BLOOD_DRAIN, SKILLID_NODE(SKILL_BLOOD_DRAIN, flag) ));
	}
	
	//-----------------------------------------------------
	// 
	
	//
	//-----------------------------------------------------
	if (g_pPlayer->IsInvisible())
	{
		flag = FLAG_SKILL_ENABLE;
		
		insert(SKILLID_MAP::value_type( MAGIC_UN_INVISIBILITY, SKILLID_NODE(MAGIC_UN_INVISIBILITY, flag) ));
	}
		
	switch( PlayerCreatureType )
	{
	case CREATURETYPE_WOLF :
		{
			if ((*g_pSkillInfoTable)[MAGIC_EAT_CORPSE].GetMP() > playerMP)
			{
				flag = 0;
			}
			else
			{
				flag = FLAG_SKILL_ENABLE;
			}
			insert(SKILLID_MAP::value_type( MAGIC_EAT_CORPSE, SKILLID_NODE(MAGIC_EAT_CORPSE, flag) ));
			
			
			if( vampireDomain.GetSkillStatus( MAGIC_HOWL ) == MSkillDomain::SKILLSTATUS_LEARNED )
			{
				if( (*g_pSkillInfoTable)[MAGIC_HOWL].GetMP() > playerMP)				
				{
					flag = 0;
				}
				else
				{
					flag = FLAG_SKILL_ENABLE;
				}
				insert(SKILLID_MAP::value_type( MAGIC_HOWL, SKILLID_NODE(MAGIC_HOWL, flag) ));
			}
		}
		break;
	case CREATURETYPE_WER_WOLF :
		{
			if( (*g_pSkillInfoTable)[SKILL_BITE_OF_DEATH].GetMP() > playerMP )
			{
				flag  = 0;
			}
			else
			{
				flag = FLAG_SKILL_ENABLE;
			}
			insert(SKILLID_MAP::value_type( SKILL_BITE_OF_DEATH, SKILLID_NODE(SKILL_BITE_OF_DEATH, flag) ));

			if( vampireDomain.GetSkillStatus( MAGIC_RAPID_GLIDING ) == MSkillDomain::SKILLSTATUS_LEARNED )
			{
				if( (*g_pSkillInfoTable)[MAGIC_RAPID_GLIDING].GetMP() > playerMP )
				{
					flag  = 0;
				}
				else
				{
					flag = FLAG_SKILL_ENABLE;
				}
				insert(SKILLID_MAP::value_type( MAGIC_RAPID_GLIDING, SKILLID_NODE(MAGIC_RAPID_GLIDING, flag) ));
			}
		}
		break;
	}
		
	//-----------------------------------------------------
	// 
	
	//
	//-----------------------------------------------------
	if (PlayerCreatureType!=CREATURETYPE_VAMPIRE_MALE1
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_FEMALE1
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_MALE2
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_FEMALE2
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_MALE3
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_FEMALE3
		// add by Coffee 2006.12.7
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_MALE4
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_FEMALE4
		//add by viva
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_MALE5
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_FEMALE5
		//add by viva
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_MALE6
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_FEMALE6
		// end 
		&& PlayerCreatureType!=CREATURETYPE_VAMPIRE_OPERATOR)
	{			
		flag = FLAG_SKILL_ENABLE;
		
		insert(SKILLID_MAP::value_type( MAGIC_UN_TRANSFORM, SKILLID_NODE(MAGIC_UN_TRANSFORM, flag) ));
	}	
#endif
}

//----------------------------------------------------------------------
// Check MP
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
MSkillSet::CheckMP()
{
#ifdef __GAME_CLIENT__
	
	
	
	SetAvailableSkills(); 

	 
#endif
}

//----------------------------------------------------------------------
//
//							MSkillDomain
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MSkillDomain::MSkillDomain()
{
	m_bNewSkill			= false;

	m_MaxLevel			= 0;
	m_MaxLearnedLevel	= -1;
	m_pLearnedSkillID	= NULL;
	m_DomainLevel		= 0;
	m_DomainExpRemain	= 0;

	m_DomainExpTable.Init( 151 );
}

MSkillDomain::~MSkillDomain()
{
	if (m_pLearnedSkillID!=NULL)
	{
		delete [] m_pLearnedSkillID;
	}


	//---------------------------------------------------------
	
	//---------------------------------------------------------
	SKILL_STEP_MAP::iterator iList = m_mapSkillStep.begin();

	while (iList != m_mapSkillStep.end())
	{
		SKILL_STEP_LIST* pList = iList->second;

		if (pList != NULL)
		{
			delete pList;
		}
		
		iList ++;
	}

	m_mapSkillStep.clear();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set Max Level
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MSkillDomain::SetMaxLevel()
{
	if (m_pLearnedSkillID!=NULL)
	{
		delete [] m_pLearnedSkillID;
	}

	m_pLearnedSkillID = new ACTIONINFO [m_MaxLevel+1];	
	m_MaxLearnedLevel = -1;

	for (int i=0; i<=m_MaxLevel; i++)
	{
		m_pLearnedSkillID[i] = MAX_ACTIONINFO;//ACTIONINFO_NULL;
	}
}

//----------------------------------------------------------------------
// Clear
//----------------------------------------------------------------------
void			
MSkillDomain::Clear()
{ 
//	SKILLID_MAP::iterator	iSkill = m_mapSkillID.begin();
//	SKILLID_MAP::iterator	endItr = m_mapSkillID.end();
//
//	while (iSkill!=endItr)
//	{		
//		//-----------------------------------------------

//		//-----------------------------------------------
//		(*g_pSkillAvailable).RemoveSkill( (*iSkill).first );
//
//		iSkill++;
//	}
//
//	m_iterator = NULL; 
//	m_mapSkillID.clear();

	ClearSkillList();

	//-----------------------------------------------
	
	//-----------------------------------------------
	if (m_pLearnedSkillID!=NULL)
	{
		delete [] m_pLearnedSkillID;
		
		m_MaxLevel = 0;
		m_MaxLearnedLevel = -1;	
		m_pLearnedSkillID = NULL;
	}
}

void			
MSkillDomain::ClearSkillList()
{ 
	SKILLID_MAP::iterator	iSkill = m_mapSkillID.begin();
	SKILLID_MAP::iterator	endItr = m_mapSkillID.end();

	while (iSkill!=endItr)
	{		
		//-----------------------------------------------
		
		//-----------------------------------------------
		(*g_pSkillAvailable).RemoveSkill( (*iSkill).first );

		iSkill++;
	}

	m_mapSkillID.clear();

	if(m_pLearnedSkillID != NULL)
	{
		delete [] m_pLearnedSkillID;
		m_pLearnedSkillID = NULL;
	}
}

//----------------------------------------------------------------------
// AddSkill
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool			
MSkillDomain::SetRootSkill(ACTIONINFO id, bool reset)
{
	//--------------------------------------------------
	
	//--------------------------------------------------
	//Clear();
	if(reset)
		m_DomainLevel		= 0;

	//--------------------------------------------------
	
	//--------------------------------------------------
//	if ((*g_pSkillInfoTable)[id].GetLevel()==0)
	{
		int oldMaxLevel = m_MaxLevel;

		if( reset )
			m_MaxLevel = 0;
		bool bOK = AddSkill( id );		
		
		if (oldMaxLevel > m_MaxLevel && reset)
		{
			m_MaxLevel = oldMaxLevel;
		}

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (bOK)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			SetMaxLevel();
		}
	}

	return false;
}

//----------------------------------------------------------------------
// AddSkill
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MSkillDomain::AddSkill(ACTIONINFO id)
{
	SKILLID_MAP::iterator	iSkill;

	if(SKILL_ABERRATION == id)
		int a= 0;

	iSkill = m_mapSkillID.begin();

	while( iSkill != m_mapSkillID.end() )
	{
		if( iSkill->first == id )
		{
			break;
		}
		iSkill++;
	}
	int skillLevel = (*g_pSkillInfoTable)[id].GetLevel();

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (m_MaxLevel < skillLevel)
	{
		m_MaxLevel = skillLevel;
	}

	//-----------------------------------------------
	
	//-----------------------------------------------
	if (iSkill == m_mapSkillID.end() )
	{		
		//-----------------------------------------------
		
		//-----------------------------------------------
		if(id == SKILL_ABERRATION)
			int a =0 ;
		if (skillLevel==0)
		{
			m_mapSkillID.insert(SKILLID_MAP::value_type( id, SKILLSTATUS_NEXT ));
		}
		else
		{
			m_mapSkillID.insert(SKILLID_MAP::value_type( id, SKILLSTATUS_OTHER ));
		}	
		
		//-----------------------------------------------
		
		//-----------------------------------------------

		SKILL_STEP skillStep = (*g_pSkillInfoTable)[id].GetSkillStep();
		if(skillStep == SKILL_STEP_ETC && id == SKILL_SOUL_CHAIN)
		{
#ifdef __GAME_CLIENT__ 
			if(this == &(*g_pSkillManager)[SKILL_DOMAIN_VAMPIRE])
				AddSkillStep(SKILL_STEP_VAMPIRE_INNATE, id);
			else if( this == &(*g_pSkillManager)[SKILL_DOMAIN_OUSTERS])
				AddSkillStep(SKILL_STEP_OUSTERS_ETC, id);
			else
				AddSkillStep(SKILL_STEP_APPRENTICE, id);
#endif
		}
		else
		{
			AddSkillStep(skillStep, id);
		}

		//--------------------------------------------------
		
		//--------------------------------------------------
		const SKILLINFO_NODE::SKILLID_LIST& listNextSkill = (*g_pSkillInfoTable)[id].GetNextSkillList();

		SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill = listNextSkill.begin();

		while (iNextSkill != listNextSkill.end())
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			iSkill = m_mapSkillID.find( *iNextSkill );

			//--------------------------------------------------
			
			//--------------------------------------------------
			if (iSkill == m_mapSkillID.end())
			{
				AddSkill( *iNextSkill );
			}

			iNextSkill++;
		}

		return true;
	}

	//-----------------------------------------------
	
	//-----------------------------------------------
	return false;
}


//----------------------------------------------------------------------
// Set SkillStatus
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
// Get SkillStatus
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MSkillDomain::SKILLSTATUS		
MSkillDomain::GetSkillStatus(ACTIONINFO id) const
{
	SKILLID_MAP::const_iterator	iSkill;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iSkill = m_mapSkillID.find(id);

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iSkill == m_mapSkillID.end()) 
	{
		return SKILLSTATUS_NULL;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	return (*iSkill).second;
}

//----------------------------------------------------------------------
// Remove Skill
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
// Add NextSkill
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MSkillDomain::AddNextSkill(ACTIONINFO id)
{
	const SKILLINFO_NODE::SKILLID_LIST& listNextSkill = (*g_pSkillInfoTable)[id].GetNextSkillList();

	SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill = listNextSkill.begin();

	while (iNextSkill != listNextSkill.end())
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		SKILLID_MAP::iterator iSkill = m_mapSkillID.find( *iNextSkill );

		//--------------------------------------------------
		
		//--------------------------------------------------
		
		
		//--------------------------------------------------
		if (iSkill != m_mapSkillID.end())
		{
			if ((*iSkill).second==SKILLSTATUS_OTHER)
			{						
				(*iSkill).second = SKILLSTATUS_NEXT;
			}
		}

		iNextSkill++;
	}
}
void
MSkillDomain::AddNextSkillForce(ACTIONINFO id)
{
	if (id==MAX_ACTIONINFO)
	{
		return;
	}
	bool HasChildSkill = false;
//	const SKILLINFO_NODE::SKILLID_LIST& listNextSkill = (*g_pSkillInfoTable)[id].GetNextSkillList();
//
//	SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill = listNextSkill.begin();
//
//	while (iNextSkill != listNextSkill.end())
//	{
//		//--------------------------------------------------

//		//--------------------------------------------------
//		SKILLID_MAP::iterator iSkill = m_mapSkillID.find( *iNextSkill );
//
//		//--------------------------------------------------

//		//--------------------------------------------------


//		//--------------------------------------------------
//		if (iSkill != m_mapSkillID.end())
//		{
//			if ((*iSkill).second==SKILLSTATUS_LEARNED)
//			{		

//			}
//		}
//
//		iNextSkill++;
//	}
	if(!HasChildSkill && (*g_pSkillInfoTable)[id].CanDelete)
	{
		SKILLID_MAP::iterator	iSkill = m_mapSkillID.find( id );
		if (iSkill != m_mapSkillID.end())
		{
			if ((*iSkill).second==SKILLSTATUS_LEARNED)
			{						
				(*iSkill).second = SKILLSTATUS_NEXT;
			//	RemoveNextSkill(id);
			}

		}
	}
}

//----------------------------------------------------------------------
// Remove NextSkill
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MSkillDomain::RemoveNextSkill(ACTIONINFO id)
{
	if (id==MAX_ACTIONINFO)
	{
		return;
	}

	SKILLID_MAP::iterator iPreviousSkill = m_mapSkillID.find( id );

	
	ACTIONINFO previousID = (*iPreviousSkill).first;

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	const SKILLINFO_NODE::SKILLID_LIST& listNextSkill = (*g_pSkillInfoTable)[previousID].GetNextSkillList();

	SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill = listNextSkill.begin();

	while (iNextSkill != listNextSkill.end())
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		SKILLID_MAP::iterator	iSkill = m_mapSkillID.find( *iNextSkill );

		//--------------------------------------------------
		
		//--------------------------------------------------
		
		
		//--------------------------------------------------
		if (iSkill != m_mapSkillID.end())
		{
			if ((*iSkill).second==SKILLSTATUS_NEXT)
			{						
				(*iSkill).second = SKILLSTATUS_OTHER;
			}
		}

		iNextSkill++;
	}
}

//----------------------------------------------------------------------
// Learn Skill
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
bool
MSkillDomain::LearnSkill(ACTIONINFO id)
{
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (!m_bNewSkill)
	{
		return false;
	}


	//--------------------------------------------------
	
	//--------------------------------------------------
	if ((*g_pSkillInfoTable)[id].GetLevel()!=m_MaxLearnedLevel+1)
	{
	//	return false;
	}

	SKILLID_MAP::iterator	iSkill;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iSkill = m_mapSkillID.find(id);

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iSkill == m_mapSkillID.end()) 
	{
		return false;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	if ((*iSkill).second==SKILLSTATUS_LEARNED)
	{
		return false;
	}

	
	//-----------------------------------------------
	
	//-----------------------------------------------
	(*g_pSkillAvailable).AddSkill( id );

	//-----------------------------------------------
	
	//-----------------------------------------------
	int skillLevel = (*g_pSkillInfoTable)[id].GetLevel();

	
	m_pLearnedSkillID[skillLevel] = id;

	if (skillLevel > m_MaxLearnedLevel)
	{
		m_MaxLearnedLevel = skillLevel;

		//--------------------------------------------------
		//
		
		//
		//--------------------------------------------------
		
		//--------------------------------------------------
		//if (m_MaxLearnedLevel > 0)
		//{	
			///RemoveNextSkill( m_pLearnedSkillID[m_MaxLearnedLevel-1] );
		//}

		//--------------------------------------------------
		
		//--------------------------------------------------
		SKILLID_MAP::iterator iSkill2 = m_mapSkillID.begin();

		while (iSkill2 != m_mapSkillID.end())
		{
			if ((*iSkill2).second==SKILLSTATUS_NEXT)
			{						
				(*iSkill2).second = SKILLSTATUS_OTHER;
			}

			iSkill2++;
		}

		//--------------------------------------------------
		
		//--------------------------------------------------
		(*iSkill).second = SKILLSTATUS_LEARNED;	

		//--------------------------------------------------
		//
		
		//
		//--------------------------------------------------
		AddNextSkill( id );
	}	
	else
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		(*iSkill).second = SKILLSTATUS_LEARNED;	
	}

	m_bNewSkill = false;

	return true;
}

//----------------------------------------------------------------------
// UnLearn Skill
//----------------------------------------------------------------------

//


//
//----------------------------------------------------------------------
bool
MSkillDomain::UnLearnSkill(ACTIONINFO id)
{
	//--------------------------------------------------
	
	//--------------------------------------------------
	if ((*g_pSkillInfoTable)[id].GetLevel()!=m_MaxLearnedLevel)
	{
		return false;
	}

	SKILLID_MAP::iterator	iSkill;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iSkill = m_mapSkillID.find(id);

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iSkill == m_mapSkillID.end()) 
	{
		return false;
	}

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	
	if ((*iSkill).second!=SKILLSTATUS_LEARNED)
	{
		return false;
	}
	
	//--------------------------------------------------
	
	
	//--------------------------------------------------
	RemoveNextSkill( m_pLearnedSkillID[m_MaxLearnedLevel] );

	//--------------------------------------------------
	
	//--------------------------------------------------
	m_pLearnedSkillID[m_MaxLearnedLevel] = MAX_ACTIONINFO;//ACTIONINFO_NULL;
	m_MaxLearnedLevel--;
	

	(*iSkill).second = SKILLSTATUS_OTHER;	

	//-----------------------------------------------
	
	//-----------------------------------------------
	(*g_pSkillAvailable).RemoveSkill( id );
	
	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	if (m_MaxLearnedLevel>=0)
	{
		AddNextSkill( m_pLearnedSkillID[m_MaxLearnedLevel] );
	}


	return true;
}


//----------------------------------------------------------------------
// Is Exist SkillStep
//----------------------------------------------------------------------
BOOL
MSkillDomain::IsExistSkillStep(SKILL_STEP ss) const
{
	SKILL_STEP_MAP::const_iterator iList = m_mapSkillStep.find( ss );

	if (iList == m_mapSkillStep.end())
	{
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------
// Get SkillStep List
//----------------------------------------------------------------------
const MSkillDomain::SKILL_STEP_LIST*	
MSkillDomain::GetSkillStepList(SKILL_STEP ss) const
{
	SKILL_STEP_MAP::const_iterator iList = m_mapSkillStep.find( ss );

	if (iList == m_mapSkillStep.end())
	{
		return NULL;
	}

	return iList->second;
}

//----------------------------------------------------------------------
// Add SkillStep List
//----------------------------------------------------------------------
void
MSkillDomain::AddSkillStep(SKILL_STEP ss, ACTIONINFO ai)
{
	SKILL_STEP_LIST* pList;
	SKILL_STEP_MAP::const_iterator iList = m_mapSkillStep.find( ss );

	if (iList == m_mapSkillStep.end())
	{
		
		pList = new SKILL_STEP_LIST;
	}
	else
	{
		pList = iList->second;
	}

	
	SKILL_STEP_LIST list = *pList;
	bool bExist = false;
	for(int i=0;i<list.size();i++)
	{
		if( list[i] == ai )
		{			
			bExist = true;
			break;
		}
	}
	
	if(bExist == false)
//		pList->push_back( ai );	
	{
		
		if( pList->empty() )
		{
			pList->push_back( ai );
		} else
		{
			bool	bAdded = false;
			SKILL_STEP_LIST::iterator itr = pList->begin();
			SKILL_STEP_LIST::iterator endItr = pList->end();	
			int		LearnLevel = (*g_pSkillInfoTable)[ai].GetLearnLevel();
			while( itr != endItr )
			{
				int currentPositionLevel = (*g_pSkillInfoTable)[*itr].GetLearnLevel();
				
				if( currentPositionLevel > LearnLevel )
				{
					pList->insert( itr, ai );
					bAdded = true;
					break;
				}
				
				itr++;
			}
			if(bAdded == false )
			{
				pList->push_back( ai );
			}
		}
	}

	
	m_mapSkillStep[ss] = pList;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MSkillDomain::SaveToFile(std::ofstream& file)
{
	SKILLID_MAP::iterator	iSkill = m_mapSkillID.begin();

	
	int size = m_mapSkillID.size();
	file.write((const char*)&size, 4);

	
	while (iSkill != m_mapSkillID.end())
	{
		WORD id = (*iSkill).first;
		BYTE status = (*iSkill).second;

		file.write((const char*)&id, 2);
		file.write((const char*)&status, 1);

		iSkill++;
	}    
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MSkillDomain::LoadFromFile(std::ifstream& file)
{
	Clear();
	//m_mapSkillID.clear();
	
	
	int size;
	file.read((char*)&size, 4);

	
	WORD id;
	BYTE status;
	for (int i=0; i<size; i++)
	{		
		file.read((char*)&id, 2);
		file.read((char*)&status, 1);

		m_mapSkillID.insert(SKILLID_MAP::value_type( 
										(enum ACTIONINFO)id, 
										(enum SKILLSTATUS)status ));
	} 
}

//----------------------------------------------------------------------
// LoadFromFileServerDomainInfo
//----------------------------------------------------------------------
void		
MSkillDomain::LoadFromFileServerDomainInfo(std::ifstream& file)
{	
	int level;

	file.read((char*)&level, 4);

	
	m_DomainExpTable[level].LoadFromFile( file );	
}

bool		
MSkillDomain::IsAvailableDeleteSkill(ACTIONINFO id)
{	
	const SKILLINFO_NODE::SKILLID_LIST& listNextSkill = (*g_pSkillInfoTable)[id].GetNextSkillList();

	SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill = listNextSkill.begin();

	while (iNextSkill != listNextSkill.end())
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		int TempSkillID = *iNextSkill;
		if(false == (*g_pSkillInfoTable)[*iNextSkill].CanDelete)
			return false;
		SKILLID_MAP::iterator iSkill = m_mapSkillID.find( *iNextSkill );

		if (iSkill != m_mapSkillID.end())
		{
			if ((*iSkill).second==SKILLSTATUS_LEARNED)
			{						
				return false;
			}
		}

		iNextSkill++;
	}
	return true;
}

//----------------------------------------------------------------------
// Get ExpInfo
//----------------------------------------------------------------------
const ExpInfo&	
MSkillDomain::GetExpInfo(int level) const
{
	return m_DomainExpTable[level];
}

//----------------------------------------------------------------------
//
// MSkillManager
//
//----------------------------------------------------------------------
MSkillManager::MSkillManager()
{
}

MSkillManager::~MSkillManager()
{
}

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
MSkillManager::Init()
{
	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	CTypeTable<MSkillDomain>::Init( MAX_SKILLDOMAIN );
	//--------------------------------------------------
	
	//--------------------------------------------------

	m_pTypeInfo[SKILLDOMAIN_BLADE].SetRootSkill( SKILL_SINGLE_BLOW );
	m_pTypeInfo[SKILLDOMAIN_SWORD].SetRootSkill( SKILL_DOUBLE_IMPACT );
	m_pTypeInfo[SKILLDOMAIN_GUN].SetRootSkill( SKILL_FAST_RELOAD );
	m_pTypeInfo[SKILLDOMAIN_ENCHANT].SetRootSkill( MAGIC_CREATE_HOLY_WATER );
	m_pTypeInfo[SKILLDOMAIN_HEAL].SetRootSkill( MAGIC_CURE_LIGHT_WOUNDS );
	m_pTypeInfo[SKILLDOMAIN_VAMPIRE].SetRootSkill( MAGIC_HIDE );
	m_pTypeInfo[SKILLDOMAIN_OUSTERS].SetRootSkill( SKILL_FLOURISH );
	m_pTypeInfo[SKILLDOMAIN_ETC].SetRootSkill( SKILL_SOUL_CHAIN );


	#ifdef __GAME_CLIENT__
		//------------------------------------------------
		
		//------------------------------------------------
		std::ifstream serverDomainInfoFile;//(FILE_INFO_skill, ios::binary);
		if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_SKILL_DOMAIN_EXP").c_str(), serverDomainInfoFile))
			return;

		LoadFromFileServerDomainInfo( serverDomainInfoFile );
		serverDomainInfoFile.close();
	#endif
}

void
MSkillManager::InitSkillList()
{
	for(int i = SKILLDOMAIN_BLADE; i < MAX_SKILLDOMAIN; i++ )
		m_pTypeInfo[i].ClearSkillList();

	m_pTypeInfo[SKILLDOMAIN_BLADE].SetRootSkill( SKILL_SINGLE_BLOW , false);
	m_pTypeInfo[SKILLDOMAIN_SWORD].SetRootSkill( SKILL_DOUBLE_IMPACT , false);
	m_pTypeInfo[SKILLDOMAIN_GUN].SetRootSkill( SKILL_FAST_RELOAD , false);
	m_pTypeInfo[SKILLDOMAIN_ENCHANT].SetRootSkill( MAGIC_CREATE_HOLY_WATER , false);
	m_pTypeInfo[SKILLDOMAIN_HEAL].SetRootSkill( MAGIC_CURE_LIGHT_WOUNDS , false);
	m_pTypeInfo[SKILLDOMAIN_VAMPIRE].SetRootSkill( MAGIC_HIDE , false);
	m_pTypeInfo[SKILLDOMAIN_OUSTERS].SetRootSkill( SKILL_FLOURISH , false);
	m_pTypeInfo[SKILLDOMAIN_ETC].SetRootSkill( SKILL_SOUL_CHAIN , false);
}



//----------------------------------------------------------------------
// LoadFromFileServerSkillInfo
//----------------------------------------------------------------------
void		
MSkillManager::LoadFromFileServerDomainInfo(std::ifstream& file)
{
	int num, domain;

	file.read((char*)&num, 4);

	
	for (int i=0; i<num; i++)
	{		
		file.read((char*)&domain, 4);

		
		m_pTypeInfo[domain].LoadFromFileServerDomainInfo( file );
	}
}
