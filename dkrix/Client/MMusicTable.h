//----------------------------------------------------------------------
// MMusicTable.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//

//

// 
//----------------------------------------------------------------------

#ifndef	__MMUSICTABLE_H__
#define	__MMUSICTABLE_H__

#include "MTypeDef.h"
#include "CTypeTable.h"
#include "MString.h"

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class MUSICTABLE_INFO {
	public :
		MString					Filename;		
		MString					FilenameWav;	

	public :
		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);
		void			LoadFromFile(std::ifstream& file);
};

typedef CTypeTable<MUSICTABLE_INFO>	MUSIC_TABLE;
extern 	MUSIC_TABLE*	g_pMusicTable;

#endif
