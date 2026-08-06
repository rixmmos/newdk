//----------------------------------------------------------------------
// MJusticeAttackManager.h
//----------------------------------------------------------------------

//

//----------------------------------------------------------------------

#ifndef __MJUSTICE_ATTACK_MANAGER_H__
#define __MJUSTICE_ATTACK_MANAGER_H__

#pragma warning(disable:4786)

#include <map>
#include <string>

class MJusticeAttackManager {
	public :
		
		typedef std::map<std::string, DWORD>		TIME_MAP;

	public :
		MJusticeAttackManager();
		~MJusticeAttackManager();

		void		Release();
		
		void		AddCreature(const char* pName);
		bool		RemoveCreature(const char* pName);
		bool		HasCreature(const char* pName) const;

	private :
		TIME_MAP	m_Creatures;		
};

extern MJusticeAttackManager* g_pJusticeAttackManager;


#endif

