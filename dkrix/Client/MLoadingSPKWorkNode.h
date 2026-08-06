//----------------------------------------------------------------------
// MLoadingSPKWorkNode.h
//----------------------------------------------------------------------

//

//



 
//----------------------------------------------------------------------

#ifndef __MLOADINSPKWORKNODE_H__
#define __MLOADINSPKWORKNODE_H__

#include "MWorkNode.h"
#include "SpriteLib/CSpriteDef.h"
//#include "CSpritePack.h"
#include "SpriteLib/CSpritePack.h"


#define	MAX_FILENAME		256

//----------------------------------------------------------------------
// MLoadingSPKWorkNode
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MLoadingSPKWorkNode : public MWorkNode {
	public :
		MLoadingSPKWorkNode()	{ m_pSPK = NULL; m_bExecute = TRUE; }
		~MLoadingSPKWorkNode() {}

		//---------------------------------------------------
		
		//---------------------------------------------------
		void			SetSPK(CSpritePack* pSPK, const char* filename)
		{
			m_pSPK = pSPK;
			strcpy(m_SPKFilename, filename);
		}

	protected :
		CSpritePack*			m_pSPK;
		char					m_SPKFilename[MAX_FILENAME];
};

//----------------------------------------------------------------------
// Node 1
//----------------------------------------------------------------------
class MLoadingSPKWorkNode1 : public MLoadingSPKWorkNode {
	public :
		MLoadingSPKWorkNode1(const char* sfpaFilename, long filePosition) { strcpy(m_SFPAFilename, sfpaFilename); m_FilePosition = filePosition; }
		~MLoadingSPKWorkNode1() {}

		BOOL		Execute(MWorkNode*& pRemainNode);

	protected :
		char							m_SFPAFilename[MAX_FILENAME];
		long							m_FilePosition;
};

//----------------------------------------------------------------------
// Node 2
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MLoadingSPKWorkNode2 : public MLoadingSPKWorkNode {
	public :
		MLoadingSPKWorkNode2(CSpriteFilePositionArray* pSFPA) { m_pSFPA = pSFPA; }
		~MLoadingSPKWorkNode2() { if (m_pSFPA!=NULL) delete m_pSFPA; }

		BOOL		Execute(MWorkNode*& pRemainNode);
		
	protected :
		CSpriteFilePositionArray*		m_pSFPA;
};

//----------------------------------------------------------------------
// Node 3
//----------------------------------------------------------------------
class MLoadingSPKWorkNode3 : public MLoadingSPKWorkNode {
	public :
		MLoadingSPKWorkNode3(TYPE_SPRITEID spriteID, long fp) { m_SpriteID = spriteID; m_FilePosition = fp; }
		~MLoadingSPKWorkNode3() {}

		BOOL		Execute(MWorkNode*& pRemainNode);
		
	protected :	
		TYPE_SPRITEID			m_SpriteID;
		long					m_FilePosition;
};

//----------------------------------------------------------------------
// Node 4
//----------------------------------------------------------------------
class MLoadingSPKWorkNode4 : public MLoadingSPKWorkNode {
	public :
		MLoadingSPKWorkNode4(TYPE_SPRITEID fID, TYPE_SPRITEID lID, long fp) { m_FirstSpriteID = fID; m_LastSpriteID = lID; m_FilePosition = fp; }
		~MLoadingSPKWorkNode4() {}

		BOOL		Execute(MWorkNode*& pRemainNode);
		

	protected :
		TYPE_SPRITEID			m_FirstSpriteID;
		TYPE_SPRITEID			m_LastSpriteID;
		long					m_FilePosition;
};

#endif

