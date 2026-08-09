#include <fstream>
//----------------------------------------------------------------------
// CSpriteDef.h
//----------------------------------------------------------------------
#ifndef	__CSPRITEDEF_H__
#define	__CSPRITEDEF_H__

#pragma warning(disable:4786)

#include <list>
#include <cstdint>
#include "DrawTypeDef.h"
#include "TArray.h"

// int list
typedef	std::list<int>	INT_LIST;

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class SPRITE_FILEPOSITION_NODE
{
	public :
		TYPE_SPRITEID		SpriteID;
		int32_t				FilePosition;

	public :
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);

};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
typedef	TArray<SPRITE_FILEPOSITION_NODE, TYPE_SPRITEID>	CSpriteFilePositionArray;



#endif
