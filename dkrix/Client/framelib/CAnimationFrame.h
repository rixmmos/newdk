//----------------------------------------------------------------------
// CAnimationFrame.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//


//
//----------------------------------------------------------------------
//
// [ File I/O ]
//





//
//----------------------------------------------------------------------

#ifndef	__CANIMATIONFRAME_H__
#define	__CANIMATIONFRAME_H__

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include "DrawTypeDef.h"
#include <fstream>
using namespace std;


class CAnimationFrame {
	public :
		CAnimationFrame(BYTE bltType=BLT_NORMAL);
		~CAnimationFrame();

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		void			SetFrameID(TYPE_FRAMEID FrameID, BYTE max)	{ m_FrameID = FrameID; m_MaxFrame=max; m_CurrentFrame=0; }
		TYPE_FRAMEID	GetFrameID() const		{ return m_FrameID; }
		BYTE			GetFrame() const		{ return m_CurrentFrame; }
		BYTE			GetMaxFrame() const		{ return m_MaxFrame; }

		// 
		void			NextFrame()		 		{ if (++m_CurrentFrame==m_MaxFrame) m_CurrentFrame=0; }

		//--------------------------------------------------------
		// file I/O
		//--------------------------------------------------------
		void	SaveToFile(std::ofstream& file);
		void	LoadFromFile(std::ifstream& file);

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void	SetBltType(BYTE bltType)	{ m_BltType = bltType; }
		BYTE	GetBltType() const			{ return m_BltType; }
		bool	IsBltTypeNormal() const		{ return m_BltType==BLT_NORMAL; }
		bool	IsBltTypeEffect() const		{ return m_BltType==BLT_EFFECT; }
		bool	IsBltTypeShadow() const		{ return m_BltType==BLT_SHADOW; }
		bool	IsBltTypeScreen() const		{ return m_BltType==BLT_SCREEN; }


	protected :		
		
		TYPE_FRAMEID		m_FrameID;

		
		BYTE				m_CurrentFrame;	// Current Frame
		BYTE				m_MaxFrame;		// Max Frame
	
		
		BYTE				m_BltType;
};

#endif

