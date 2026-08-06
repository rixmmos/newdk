//----------------------------------------------------------------------
// MHelicopterManager.h
//----------------------------------------------------------------------


//----------------------------------------------------------------------

#ifndef __MHELICOPTERMANAGER_H__
#define __MHELICOPTERMANAGER_H__

#include "MTypeDef.h"
#include "CTypeMap2.h"
class ZONESOUND_NODE;

//----------------------------------------------------------------------
// MHelicopter
//----------------------------------------------------------------------
class MHelicopter {
	public :
		MHelicopter();
		~MHelicopter();

		void			Release();

		bool			SetChaseCreature(TYPE_OBJECTID id, int x0, int y0);		

		TYPE_OBJECTID	GetCreatureID() const	{ return m_CreatureID; }
		TYPE_OBJECTID	GetEffectID() const		{ return m_EffectID; }

		void			UpdateSound(int x, int y);

	protected :
		TYPE_OBJECTID			m_CreatureID;	
		TYPE_OBJECTID			m_EffectID;		
		ZONESOUND_NODE*			m_pSound;		
};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MHelicopterManager : private CTypeMap2<MHelicopter> {
	public :
		MHelicopterManager();
		~MHelicopterManager();

		void			Release();

		bool			AddHelicopter(MHelicopter* pHelicopter);
		MHelicopter*	GetHelicopter(TYPE_OBJECTID creatureID) const;
		void			RemoveHelicopter(TYPE_OBJECTID creatureID);
		void			RemoveHelicopterSoon(TYPE_OBJECTID creatureID);

		void			Update();		
};

#endif
