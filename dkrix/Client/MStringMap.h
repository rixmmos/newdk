//----------------------------------------------------------------------
// MStringMap.h
//----------------------------------------------------------------------

//




//----------------------------------------------------------------------

#ifndef __MSTRINGMAP_H__
#define __MSTRINGMAP_H__

#pragma warning(disable:4786)

#include "MString.h"

#include <fstream>
#include <map>
using namespace std;

//----------------------------------------------------------------------


//----------------------------------------------------------------------
class MStringPointerCompare {
	public :
		bool operator () ( MString* pLeft, MString* pRight ) const
		{
			return *pLeft < *pRight;
		}
};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MStringMap : public std::map<MString*, MString*, MStringPointerCompare> {
	public :
		typedef std::map<MString*, MString*, MStringPointerCompare>	STRING_MAP;
		typedef STRING_MAP::iterator			iterator;
		typedef STRING_MAP::const_iterator		const_iterator;

	public :
		MStringMap();
		~MStringMap();

		//-------------------------------------------------------
		// Release
		//-------------------------------------------------------
		void				Release();

		//-------------------------------------------------------
		// Add / Remove / Get
		//-------------------------------------------------------
		bool				Add(const char* pKey, const char* pValue=NULL);
		bool				Remove(const char* pKey);
		const MString*		Get(const char* pKey) const;

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void				SaveToFile(std::ofstream& file);
		void				LoadFromFile(std::ifstream& file);

	protected :
		
};

#endif

