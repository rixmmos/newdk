//----------------------------------------------------------------------
// MObjectSelector
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------

#ifndef __MOBJECTSELECTOR__
#define __MOBJECTSELECTOR__

class MCreature;

class MObjectSelector {
	public :
		enum SELECT_TARGET
		{
			SELECT_ALL,
			SELECT_FRIEND,
			SELECT_ENEMY
		};

		enum SELECT_BY
		{
			SELECT_BY_RACE,
			SELECT_BY_GUILD,
		};


	public :
		MObjectSelector();
		~MObjectSelector();

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		void	SelectAll()			{ m_SelectTarget = SELECT_ALL; }		
		void	SelectFriend()		{ m_SelectTarget = SELECT_FRIEND; }
		void	SelectEnemy()		{ m_SelectTarget = SELECT_ENEMY; }

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		void	SelectByRace()		{ m_SelectBy = SELECT_BY_RACE; }
		void	SelectByGuild()		{ m_SelectBy = SELECT_BY_GUILD; }

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		BOOL	CanSelect(MCreature* pCreature);

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		BOOL	CanAttack(MCreature* pCreature);

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		BOOL	IsWarEnemy(MCreature* pCreature);

		
//		BOOL	IsAblePKbyZone(MCreature* pCreature);

	protected :
		SELECT_TARGET		m_SelectTarget;
		SELECT_BY			m_SelectBy;
};

extern MObjectSelector* g_pObjectSelector;

#endif

