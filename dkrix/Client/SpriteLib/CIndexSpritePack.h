#include <fstream>
//----------------------------------------------------------------------
// CIndexSpritePack.h
//----------------------------------------------------------------------
//
// SpritePack의 Sprite에 대한 File Pointer Index가 필요하다.
// SpriteSet에서 읽기 위해서.
//
//----------------------------------------------------------------------

#ifndef	__CINDEXSPRITEPACK_H__
#define	__CINDEXSPRITEPACK_H__

#include "CIndexSprite.h"
#include "CTypePack.h"

// Phase 4A: see CSpritePack.h for the single-type typedef rationale.
typedef CTypePack2<CIndexSprite, CIndexSprite, CIndexSprite> CIndexSpritePack;

//class CIndexSpritePack : public CTypePack<CIndexSprite>
//{
//public:
//	//--------------------------------------------------------
//	// Init/Release
//	//--------------------------------------------------------
//	void	Init(WORD size, bool b565 = true);
//	
//	//--------------------------------------------------------
//	// file I/O
//	//--------------------------------------------------------
//	bool LoadFromFile(std::ifstream &file, bool b565 = true);
//	bool LoadFromFile(LPCTSTR lpszFilename, bool b565 = true);
//	bool LoadFromFileRunning(LPCTSTR lpszFilename, bool b565 = true);
//	
//protected:
//	bool			m_b565;
//};

#endif


