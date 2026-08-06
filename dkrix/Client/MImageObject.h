//----------------------------------------------------------------------
// MImageObject.h
//----------------------------------------------------------------------
//

// 




//




//


//


//

//
//----------------------------------------------------------------------
//

//


//


//----------------------------------------------------------------------
//



// 
// 
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------

#ifndef	__MIMAGEOBJECT_H__
#define	__MIMAGEOBJECT_H__

#include <fstream>
#include "MTypeDef.h"
#include "MObject.h"
#include "DrawTypeDef.h"

using namespace std;

#define	IMAGEOBJECT_NULL	0xFFFF
#define OBJECT_TRANS_FLAG	1
#define OBJECT_HALF_FLAG	2
//----------------------------------------------------------------------
//
// ImageObject class
//
//----------------------------------------------------------------------
class MImageObject : public MObject {
	public :
		
		enum WALL_DIRECTION
		{
			WALL_RIGHTDOWN = 1,		
			WALL_RIGHTUP,			

			WALL_NULL = SECTORPOSITION_NULL,			
		};

	public :
		MImageObject();
		MImageObject(TYPE_OBJECTID id, TYPE_OBJECTID ImageObjectID, TYPE_SPRITEID nSprite, int pX, int pY, TYPE_SECTORPOSITION viewpoint, BYTE trans);
		~MImageObject();
		
		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		virtual void	SaveToFile(std::ofstream& file);
		virtual void	LoadFromFile(std::ifstream& file);

		//-------------------------------------------------------
		// Is...
		//-------------------------------------------------------
		bool	IsAnimation() const		{ return m_bAnimation; }
		bool	IsSame(const MImageObject* pImageObject) const	{ return m_ImageObjectID==pImageObject->m_ImageObjectID; }
		//bool	IsTrans() const			{ return m_bTrans; }
		bool	IsTrans() const			{ return (m_bTrans&OBJECT_TRANS_FLAG)?true:false; }
		bool	IsHalf() const			{ return (m_bTrans&OBJECT_HALF_FLAG)?true:false; }
		//-------------------------------------------------------
		// set				
		//-------------------------------------------------------
		void	Set(TYPE_OBJECTID ImageObjectID, TYPE_SPRITEID SpriteID, int pX, int pY, TYPE_SECTORPOSITION viewpoint, BYTE trans);
		void	SetPixelX(int px)		{ m_PixelX = px; }
		void	SetPixelY(int py)		{ m_PixelY = py; }
		void	SetViewpoint(TYPE_SECTORPOSITION vp)	{ m_Viewpoint = vp; }
		//void	SetTrans()				{ m_bTrans = true; }
		//void	UnSetTrans()				{ m_bTrans = false; }
		void	SetTrans()				{ m_bTrans |= OBJECT_TRANS_FLAG; }
		void	UnSetTrans()				{ m_bTrans &= ~OBJECT_TRANS_FLAG; }
		void	SetHalf()				{ m_bTrans |= OBJECT_HALF_FLAG; }
		void	UnSetHalf()				{ m_bTrans &= ~OBJECT_HALF_FLAG; }

		//-------------------------------------------------------
		// get
		//-------------------------------------------------------
		TYPE_OBJECTID			GetImageObjectID()		{ return m_ImageObjectID; }
		TYPE_SPRITEID	GetSpriteID() const		{ return m_SpriteID; }
		int				GetPixelX()		const	{ return m_PixelX; }
		int				GetPixelY()		const	{ return m_PixelY; }
		TYPE_SECTORPOSITION			GetViewpoint() const	{ return m_Viewpoint; }
		
		//-------------------------------------------------------
		
		//-------------------------------------------------------

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void				SetWall(WALL_DIRECTION wd);
		bool				IsWall() const;
		WALL_DIRECTION		GetWallDirection() const;		
		int					GetWallValue() const;
		bool				IsWallTransPosition(int sX, int sY) const;
		

	protected :

		
		
		TYPE_OBJECTID			m_ImageObjectID;

		
		TYPE_SPRITEID			m_SpriteID;

		
		bool					m_bAnimation;

		
		int						m_PixelX;
		int						m_PixelY;

		
		TYPE_SECTORPOSITION					m_Viewpoint;
		
		
		BYTE					m_bTrans;
};


#endif


