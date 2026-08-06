//----------------------------------------------------------------------
// CFrameSet.h
//----------------------------------------------------------------------
//

//


//


//
//----------------------------------------------------------------------

#ifndef	__CFRAMESET_H__
#define	__CFRAMESET_H__

#include "CFrame.h"
#include <fstream>
#include <cstdint>


template <class Type>
class CFrameSet {
	public :
		CFrameSet();
		~CFrameSet();

		//--------------------------------------------------------
		// Init/Release
		//--------------------------------------------------------
		void		Init(TYPE_FRAMEID count);		
		void		Release();

		//--------------------------------------------------------
		// file I/O
		//--------------------------------------------------------
		
		// indexFile = FilePointer File, packFile = FramePack File
		bool		LoadFromFile(std::ifstream& indexFile, std::ifstream& packFile);
		

		//--------------------------------------------------------
		// operator
		//--------------------------------------------------------
		Type&	operator [] (TYPE_FRAMEID n) { return m_pFrames[n]; }


	protected :
		TYPE_FRAMEID		m_nFrames;			
		Type*				m_pFrames;			
};


//----------------------------------------------------------------------
// CFrameSet.cpp
//----------------------------------------------------------------------

//#include "CFramePack.h"
//#include "CFrameSet.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
template <class Type>
CFrameSet<Type>::CFrameSet()
{
	m_nFrames		= 0;
	m_pFrames		= NULL;
}

template <class Type>
CFrameSet<Type>::~CFrameSet()
{
	
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
template <class Type>
void
CFrameSet<Type>::Init(TYPE_FRAMEID count)
{
	
	if (count==0) 
		return;

	
	Release();

	
	m_nFrames = count;

	m_pFrames = new Type [m_nFrames];
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class Type>
void
CFrameSet<Type>::Release()
{
	if (m_pFrames != NULL)
	{
		
		delete [] m_pFrames;
		m_pFrames = NULL;

		m_nFrames = 0;
	}
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <class Type>
bool		
CFrameSet<Type>::LoadFromFile(std::ifstream& indexFile, std::ifstream& packFile)
{
	TYPE_FRAMEID	count;
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	indexFile.read((char*)&count, SIZE_FRAMEID);


	int32_t* pIndex = new int32_t [count];	// file position (32-bit)

	//------------------------------------------------------
	
	//------------------------------------------------------
	for (TYPE_FRAMEID i=0; i<count; i++)
	{
		pIndex[i] = 0;
		indexFile.read((char*)&pIndex[i], 4);
	}

	
	
	


	//------------------------------------------------------
	
	//------------------------------------------------------
	Init( count );

	//------------------------------------------------------
	
	
	//------------------------------------------------------
	for (TYPE_FRAMEID i=0; i<count; i++)
	{
		packFile.seekg(static_cast<std::streamoff>(pIndex[i]), std::ios::beg);
		m_pFrames[i].LoadFromFile( packFile );
	}

	delete [] pIndex;

	return true;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
typedef CFrameSet<FRAME_ARRAY>			CThingFrameSet;
typedef	CFrameSet<ACTION_FRAME_ARRAY>	CCreatureFrameSet;


#endif

