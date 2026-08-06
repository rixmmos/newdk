//----------------------------------------------------------------------
// MStatusManager.h
//----------------------------------------------------------------------


#ifndef __MSTATUSMANAGER_H__
#define	__MSTATUSMANAGER_H__

#include "DllInfo.h"

class __EX MStatusManager {
	public :
		MStatusManager();
		~MStatusManager();

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		//Edit by Sonic Start 2006.9.6
		void		Set(int str,int dex, int intel);
		void		SetCurrentWeaponDomain(int domain, int level);
		 
		//Edit by sonic End 2006.9.6
		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int			GetTOHIT();
		int			GetMinDAM();		// min
		int			GetMaxDAM();		// max
		int			GetDefense();
		int			GetProtection();
		int			GetAttackSpeed();
		//int			GetCC();

	protected :
		int			m_STR;
		int			m_DEX;
		int			m_INT;

		int			m_Domain;
		int			m_DomainLevel;
};

extern __EX MStatusManager		g_StatusManager;

#endif

