//----------------------------------------------------------------------
// CTypeMap2.h
//----------------------------------------------------------------------

//



//----------------------------------------------------------------------

#ifndef __CTYPEMAP2_H__
#define	__CTYPEMAP2_H__

#pragma warning(disable:4786)

#include <map>

//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <class DataType>
class CTypeMap2 : public std::map<unsigned int, DataType*> {
	public :
		typedef	std::map<unsigned int, DataType*>	TYPE_MAP;

		// Bring base class methods into scope
		using std::map<unsigned int, DataType*>::begin;
		using std::map<unsigned int, DataType*>::end;
		using std::map<unsigned int, DataType*>::find;
		using std::map<unsigned int, DataType*>::clear;
		using std::map<unsigned int, DataType*>::insert;

	public :
		CTypeMap2();
		virtual ~CTypeMap2();

		//-----------------------------------------------------
		// Release
		//-----------------------------------------------------
		virtual void		Release();

		//-----------------------------------------------------
		// Item Info
		//-----------------------------------------------------
		virtual bool		AddData( unsigned int id, DataType* pData );
		virtual DataType*	GetData( unsigned int id );
		virtual const DataType*	GetData( unsigned int id ) const;
		virtual bool		RemoveData( unsigned int id );

	protected :
};



//----------------------------------------------------------------------
//
// CTypeMap2
//
//----------------------------------------------------------------------
template <class DataType>
CTypeMap2<DataType>::CTypeMap2()
{
}

template <class DataType>		
CTypeMap2<DataType>::~CTypeMap2()
{
	Release();
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class DataType>
void
CTypeMap2<DataType>::Release()
{
	typename TYPE_MAP::iterator iData = begin();

	while (iData != end())
	{
		DataType*	pData	= (*iData).second;		

		delete pData;

		iData ++;
	}

	clear();
}

//----------------------------------------------------------------------
// Add Data
//----------------------------------------------------------------------
template <class DataType>
bool				
CTypeMap2<DataType>::AddData( unsigned int id, DataType* pData )
{
	typename TYPE_MAP::iterator	iData = find( id );

	if (iData != end())
	{
		
		
		return false;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	insert(typename TYPE_MAP::value_type(id, pData));

	return true;
}

//----------------------------------------------------------------------
// Get Data
//----------------------------------------------------------------------
template <class DataType>
DataType*	
CTypeMap2<DataType>::GetData( unsigned int id )
{
	typename TYPE_MAP::iterator	iData = find( id );

	if (iData == end())
	{
		
		return NULL;
	}

	
	return (*iData).second;
}

//----------------------------------------------------------------------
// Get Data
//----------------------------------------------------------------------
template <class DataType>
const DataType*	
CTypeMap2<DataType>::GetData( unsigned int id ) const
{
	typename TYPE_MAP::const_iterator	iData = find( id );

	if (iData == end())
	{
		
		return NULL;
	}

	
	return (*iData).second;
}

//----------------------------------------------------------------------
// Remove NPC Info
//----------------------------------------------------------------------
template <class DataType>
bool				
CTypeMap2<DataType>::RemoveData( unsigned int id )
{
	typename TYPE_MAP::iterator	iData = find( id );

	if (iData == end())
	{
		
		return false;
	}

	
	delete (*iData).second;

	TYPE_MAP::erase( iData );

	return true;
}


#endif

