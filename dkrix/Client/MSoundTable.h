//----------------------------------------------------------------------
// MSoundTable.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//

//

// 
//----------------------------------------------------------------------

#ifndef	__MSOUNDTABLE_H__
#define	__MSOUNDTABLE_H__

#include "MTypeDef.h"
#include "CTypeTable.h"
#include "MString.h"
#include "SoundDef.h"
#include <fstream>

using namespace std;


//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class SOUNDTABLE_INFO {
	public :
		SOUNDTABLE_INFO()
		{
			//pDSBuffer = NULL;			
		}

		~SOUNDTABLE_INFO()
		{
			//if (pDSBuffer != NULL)			
			//{
			//	delete pDSBuffer;
			//}			
		}

	public :
		MString						Filename;	
		

	public :
		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(std::ifstream& file);		
};

typedef CTypeTable<SOUNDTABLE_INFO>		SOUND_TABLE;
extern 	SOUND_TABLE*		g_pSoundTable;


#endif
