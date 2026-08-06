//----------------------------------------------------------------------
// MInteractionObject.h
//----------------------------------------------------------------------
//

// 
//----------------------------------------------------------------------
//


//
//----------------------------------------------------------------------

#ifndef	__MINTERACTIONOBJECT_H__
#define	__MINTERACTIONOBJECT_H__

#include "MTypeDef.h"
#include "MAnimationObject.h"

#include <fstream>
using namespace std;


//----------------------------------------------------------------------
//
// InteractionObject class
//
//----------------------------------------------------------------------
class MInteractionObject : public MAnimationObject {
	public :
		MInteractionObject();
		MInteractionObject(TYPE_INTERACTIONOBJECTTYPE iaotype, TYPE_OBJECTID id, TYPE_OBJECTID ImageObjectID, TYPE_SPRITEID nSprite, int pX, int pY, TYPE_SECTORPOSITION viewpoint, bool trans, BYTE type=BLT_NORMAL);
		~MInteractionObject();

		//--------------------------------------------------------
		
		//--------------------------------------------------------		
		void			SetAction(BYTE action);
		BYTE			GetAction() const			{ return m_CurrentFrame; }

		//--------------------------------------------------------		
		
		//--------------------------------------------------------		
		void			SetNextAction(BYTE action);

		//--------------------------------------------------------		
		// InteractionObject type
		//--------------------------------------------------------		
		BYTE			GetInteractionObjectType() const	{ return m_InteractionObjectType; }

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		virtual void	SaveToFile(std::ofstream& file);
		virtual void	LoadFromFile(std::ifstream& file);

	public :
		enum INTERACTIONOBJECT_TYPE 
		{
			TYPE_DOOR = 0,
			TYPE_TRAP,
			TYPE_SWITCH,
			TYPE_MAX
		};
		
	protected :
		
		void			ChangeActionDoor();
		void			ChangeActionTrap();
		void			ChangeActionSwitch();

	protected :
		BYTE			m_InteractionObjectType;
};


#endif


