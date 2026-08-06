//----------------------------------------------------------------------
// MStatus.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSTATUS_H__
#define	__MSTATUS_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include "Packet/Types/ModifyDef.h"

class MStatus {
	public :
		MStatus();
		~MStatus();

		//-------------------------------------------
		
		//-------------------------------------------
		void	ClearStatus();

		//-------------------------------------------
		
		//-------------------------------------------
		virtual void	SetStatus(DWORD n, DWORD value)	{ m_Status[n] = value; }

		//-------------------------------------------
		
		//-------------------------------------------
		DWORD	GetStatus(DWORD n) const		{ return m_Status[n]; }
		
		//-------------------------------------------
		// Get
		//-------------------------------------------
		DWORD	GetBASIC_STR() const	{ return m_Status[MODIFY_BASIC_STR]; }
		DWORD	GetSTR() const			{ return m_Status[MODIFY_CURRENT_STR]; }
		DWORD	GetMAX_STR() const			{ return m_Status[MODIFY_MAX_STR]; }
		DWORD	GetSTR_EXP() const			{ return m_Status[MODIFY_STR_EXP_REMAIN]; }

		DWORD	GetBASIC_DEX() const	{ return m_Status[MODIFY_BASIC_DEX]; }
		DWORD	GetDEX() const			{ return m_Status[MODIFY_CURRENT_DEX]; }
		DWORD	GetMAX_DEX() const			{ return m_Status[MODIFY_MAX_DEX]; }
		DWORD	GetDEX_EXP() const			{ return m_Status[MODIFY_DEX_EXP_REMAIN]; }

		DWORD	GetBASIC_INT() const	{ return m_Status[MODIFY_BASIC_INT]; }
		DWORD	GetINT() const			{ return m_Status[MODIFY_CURRENT_INT]; }
		DWORD	GetMAX_INT() const			{ return m_Status[MODIFY_MAX_INT]; }
		DWORD	GetINT_EXP() const			{ return m_Status[MODIFY_INT_EXP_REMAIN]; }

		DWORD	GetHP() const			{ return m_Status[MODIFY_CURRENT_HP]; }
		DWORD	GetMAX_HP() const			{ return m_Status[MODIFY_MAX_HP]; }

		DWORD	GetMP() const			{ return m_Status[MODIFY_CURRENT_MP]; }
		DWORD	GetMAX_MP() const			{ return m_Status[MODIFY_MAX_MP]; }

		DWORD	GetMIN_DAMAGE() const			{ return m_Status[MODIFY_MIN_DAMAGE]; }
		DWORD	GetMAX_DAMAGE() const			{ return m_Status[MODIFY_MAX_DAMAGE]; }

		DWORD	GetDefense() const			{ return m_Status[MODIFY_DEFENSE]; }
		DWORD	GetProtection() const			{ return m_Status[MODIFY_PROTECTION]; }
		DWORD	GetTOHIT() const			{ return m_Status[MODIFY_TOHIT]; }

		DWORD	GetVision() const			{ return m_Status[MODIFY_VISION]; }
		DWORD	GetFAME() const			{ return m_Status[MODIFY_FAME]; }
		DWORD	GetGold() const			{ return m_Status[MODIFY_GOLD]; }

		DWORD	GetSWORD_DOMAIN_LEVEL() const			{ return m_Status[MODIFY_SWORD_DOMAIN_LEVEL]; }
		DWORD	GetBLADE_DOMAIN_LEVEL() const			{ return m_Status[MODIFY_BLADE_DOMAIN_LEVEL]; }
		DWORD	GetHEAL_DOMAIN_LEVEL() const			{ return m_Status[MODIFY_HEAL_DOMAIN_LEVEL]; }
		DWORD	GetENCHANT_DOMAIN_LEVEL() const			{ return m_Status[MODIFY_ENCHANT_DOMAIN_LEVEL]; }
		DWORD	GetGUN_DOMAIN_LEVEL() const			{ return m_Status[MODIFY_GUN_DOMAIN_LEVEL]; }
		DWORD	GetETC_DOMAIN_LEVEL() const			{ return m_Status[MODIFY_ETC_DOMAIN_LEVEL]; }

		DWORD	GetSWORD_DOMAIN_EXP() const			{ return m_Status[MODIFY_SWORD_DOMAIN_EXP_REMAIN]; }     

		DWORD	GetBLADE_DOMAIN_EXP() const			{ return m_Status[MODIFY_BLADE_DOMAIN_EXP_REMAIN]; }     

		DWORD	GetHEAL_DOMAIN_EXP() const			{ return m_Status[MODIFY_HEAL_DOMAIN_EXP_REMAIN]; }      

		DWORD	GetENCHANT_DOMAIN_EXP() const			{ return m_Status[MODIFY_ENCHANT_DOMAIN_EXP_REMAIN]; }   

		DWORD	GetGUN_DOMAIN_EXP() const			{ return m_Status[MODIFY_GUN_DOMAIN_EXP_REMAIN]; }       

		DWORD	GetETC_DOMAIN_EXP() const			{ return m_Status[MODIFY_ETC_DOMAIN_EXP_REMAIN]; }       


		DWORD	GetSkillLevel() const			{ return m_Status[MODIFY_SKILL_LEVEL]; }
		DWORD	GetLEVEL() const			{ return m_Status[MODIFY_LEVEL]; }
		DWORD	GetEFFECT_STAT() const			{ return m_Status[MODIFY_EFFECT_STAT]; }
		DWORD	GetDURATION() const			{ return m_Status[MODIFY_DURATION]; }
		DWORD	GetBullet() const			{ return m_Status[MODIFY_BULLET]; }
		DWORD	GetBonusPoint() const			{ return m_Status[MODIFY_BONUS_POINT]; }
		DWORD	GetDurability() const			{ return m_Status[MODIFY_DURABILITY]; }
		DWORD	GetNotoriety() const			{ return m_Status[MODIFY_NOTORIETY]; }
		DWORD	GetVampExp() const			{ return m_Status[MODIFY_VAMP_EXP_REMAIN]; }

		DWORD	GetSilverDamage() const			{ return m_Status[MODIFY_SILVER_DAMAGE]; }        
		DWORD	GetAttackSpeed() const			{ return m_Status[MODIFY_ATTACK_SPEED]; }        
		DWORD	GetAlignment() const			{ return m_Status[MODIFY_ALIGNMENT]; }        
		DWORD	GetSilverDurability() const		{ return m_Status[MODIFY_SILVER_DURABILITY]; }	
//		DWORD	GetArmageddonHP() const			{ return m_Status[MODIFY_ARMAGEDDON_HP]; }

		DWORD	GetGrade() const				{ return m_Status[MODIFY_RANK]; }		
		DWORD	GetGradeExpRemain() const				{ return m_Status[MODIFY_RANK_EXP_REMAIN]; }	
		
		DWORD	GetOustersExp() const			{ return m_Status[MODIFY_OUSTERS_EXP_REMAIN]; }
		DWORD	GetSkillBounsPoint() const			{ return m_Status[MODIFY_SKILL_BONUS_POINT]; }

		DWORD	GetElementalFire() const			{ return m_Status[MODIFY_ELEMENTAL_FIRE]; }
		DWORD	GetElementalWater() const			{ return m_Status[MODIFY_ELEMENTAL_WATER]; }
		DWORD	GetElementalEarth() const			{ return m_Status[MODIFY_ELEMENTAL_EARTH]; }
		DWORD	GetElementalWind() const			{ return m_Status[MODIFY_ELEMENTAL_WIND]; }
		DWORD	GetAdvancementClassLevel() const { return m_Status[ MODIFY_ADVANCEMENT_CLASS_LEVEL ]; }
		DWORD	GetAdvancementClassGoalExp() const { return m_Status[ MODIFY_ADVANCEMENT_CLASS_GOAL_EXP ]; }

		//-------------------------------------------
		
		//-------------------------------------------
		virtual void	ApplyStatus(const MStatus& status);

	protected :
		DWORD		m_Status[MAX_MODIFY];		
};

#endif

