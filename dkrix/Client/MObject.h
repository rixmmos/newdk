//----------------------------------------------------------------------
// MObject.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
/* class hierachy

      MObject --+-- MCreature ---- MCreatureWear -- MPlayer
				|
				+-- MItem
	            |				
				+-- MImageObject -- MAnimationObject -- MInteractionObject
				|
				+-- MEffect --- ....
	            


*/
//----------------------------------------------------------------------

#ifndef	__MOBJECT_H__
#define	__MOBJECT_H__

#include "Client_PCH.h"
#include "MTypeDef.h"
#include <fstream>
using namespace std;


class MObject {
	public :
		MObject();
		~MObject()	{}

		BYTE	GetObjectType()	const	{ return m_ObjectType; }

		//--------------------------------------------------------
		// id
		//--------------------------------------------------------
		void			SetID(TYPE_OBJECTID id)	{ m_ID = id; }
		TYPE_OBJECTID	GetID()	const			{ return m_ID; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		void		SetPosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)	{ m_X = x; m_Y = y; }
		void		SetX(TYPE_SECTORPOSITION x)		{ m_X = x; }
		void		SetY(TYPE_SECTORPOSITION y)		{ m_Y = y; }
		TYPE_SECTORPOSITION		GetX()	const		{ return m_X; }		
		TYPE_SECTORPOSITION		GetY()	const		{ return m_Y; }		

		//------------------------------------------------------
		
		//------------------------------------------------------
		//extern BOOL g_MyFull;
		   //end edit
		void				ClearScreenRect()	
		{
				m_ScreenRect.left = 32000;	
				m_ScreenRect.top = 32000;		
				m_ScreenRect.right = 0;
				m_ScreenRect.bottom = 0;
		}
		void				AddScreenRect(RECT* pRect);
		void				SetScreenRect(RECT* pRect)	{ m_ScreenRect = *pRect; }
		const RECT&			GetScreenRect() const	{ return m_ScreenRect; }
		BOOL				IsPointInScreenRect(int x, int y) const
		{
			if (x >= m_ScreenRect.left && x < m_ScreenRect.right
				&& y >= m_ScreenRect.top && y < m_ScreenRect.bottom)
			{
				return TRUE;
			}

			return FALSE;
		}

		//--------------------------------------------------------
		// file I/O
		//--------------------------------------------------------
		virtual void	SaveToFile(std::ofstream& file);
		virtual void	LoadFromFile(std::ifstream& file);

	public :
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		enum OBJECT_TYPE 
		{
			TYPE_OBJECT = 0,
			TYPE_CREATURE,	
			TYPE_ITEM,			
			TYPE_IMAGEOBJECT,
			TYPE_SHADOWOBJECT,
			TYPE_ANIMATIONOBJECT,
			TYPE_SHADOWANIMATIONOBJECT,
			TYPE_INTERACTIONOBJECT,
			TYPE_PORTAL,
			TYPE_EFFECT			
		};

	protected :
		BYTE				m_ObjectType;	
		TYPE_OBJECTID		m_ID;			// Object's instance ID
		
		
		TYPE_SECTORPOSITION				m_X, m_Y;		

		
		RECT					m_ScreenRect;
};

#endif
