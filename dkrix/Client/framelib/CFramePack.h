//----------------------------------------------------------------------
// CFramePack.h
//----------------------------------------------------------------------

//
// typedef CFramePack<FRAME_ARRAY>				CThingFramePack;
// typedef	CFramePack<DIRECTION_FRAME_ARRAY>	CEffectFramePack;
// typedef	CFramePack<ACTION_FRAME_ARRAY>		CCreatureFramePack;
//
//



//
//----------------------------------------------------------------------

#ifndef	__CFRAMEPACK_H__
#define	__CFRAMEPACK_H__


#include "DrawTypeDef.h"
#include "CFrame.h"

template <class Type>
class CFramePack : public TArray<Type, TYPE_FRAMEID> {
	public :
		CFramePack();
		~CFramePack();

		//--------------------------------------------------------
		// File I/O
		//--------------------------------------------------------
		bool		SaveToFile(std::ofstream& packFile, std::ofstream& indexFile);

	protected :
		// Bring base class members into scope
		using TArray<Type, TYPE_FRAMEID>::m_Size;
		using TArray<Type, TYPE_FRAMEID>::m_pData;
		using TArray<Type, TYPE_FRAMEID>::s_SIZEOF_SizeType;
};


//----------------------------------------------------------------------
// CFramePack<Type>.cpp
//----------------------------------------------------------------------

//#include "CFramePack.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
template <class Type>
CFramePack<Type>::CFramePack()
{
}

template <class Type>
CFramePack<Type>::~CFramePack()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <class Type>
bool
CFramePack<Type>::SaveToFile(std::ofstream& packFile, std::ofstream& indexFile)
{
	//--------------------------------------------------
	
	//--------------------------------------------------
	packFile.write((const char*)&m_Size, s_SIZEOF_SizeType);
	indexFile.write((const char *)&m_Size, s_SIZEOF_SizeType); 

	
	if (m_pData==NULL || m_Size==0) 
		return false;

	//--------------------------------------------------
	
	//--------------------------------------------------
	long*	pIndex = new long [m_Size];

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	for (TYPE_FRAMEID i=0; i<m_Size; i++)
	{
		
		pIndex[i] = packFile.tellp();

		
		m_pData[i].SaveToFile(packFile);
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	for (TYPE_FRAMEID i=0; i<m_Size; i++)
	{
		indexFile.write((const char*)&pIndex[i], 4);
	}

	delete [] pIndex;

	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
typedef CFramePack<CFrame>							CImageFramePack;
typedef CFramePack<FRAME_ARRAY>						CAnimationFramePack;
typedef	CFramePack<DIRECTION_FRAME_ARRAY>			CDirectionFramePack;
typedef	CFramePack<DIRECTION_EFFECTFRAME_ARRAY>		CEffectFramePack;
//typedef	CFramePack<ACTION_FRAME_ARRAY>				CCreatureFramePack;

/*
class CEffectFramePack : public CFramePack<DIRECTION_EFFECTFRAME_ARRAY> {
	public :
		void		InfoToFile(const char* filename);
};
*/

class CCreatureFramePack : public CFramePack<ACTION_FRAME_ARRAY> {
	public :
		void		InfoToFile(const char* filename);
};

#endif

