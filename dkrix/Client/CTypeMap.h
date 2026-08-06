//----------------------------------------------------------------------
// CTypeMap.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef __CTYPEMAP_H__
#define	__CTYPEMAP_H__

#pragma warning(disable:4786)

#include <map>
#include <fstream>

//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <class DataType>
class CTypeMap : public std::map<unsigned int, DataType*> {
	public :
		typedef	std::map<unsigned int, DataType*>	TYPE_MAP;

	public :
		CTypeMap();
		virtual ~CTypeMap();

		//-----------------------------------------------------
		// Release
		//-----------------------------------------------------
		virtual void		Release();

		//-----------------------------------------------------
		// Item Info
		//-----------------------------------------------------
		virtual bool		AddData( unsigned int id, DataType* pData );
		virtual DataType*	GetData( unsigned int id );
		virtual bool		RemoveData( unsigned int id );

		//-----------------------------------------------------
		// File I/O
		//-----------------------------------------------------
		virtual void		SaveToFile(std::ofstream& file);
		virtual void		LoadFromFile(std::ifstream& file);

	protected :
};



//----------------------------------------------------------------------
//
// CTypeMap
//
//----------------------------------------------------------------------
template <class DataType>
CTypeMap<DataType>::CTypeMap()
{
}

template <class DataType>		
CTypeMap<DataType>::~CTypeMap()
{
	Release();
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class DataType>
void
CTypeMap<DataType>::Release()
{
	typename TYPE_MAP::iterator iData = this->begin();

	while (iData != this->end())
	{
		DataType*	pData	= (*iData).second;		

		delete pData;

		iData ++;
	}

	this->clear();
}

//----------------------------------------------------------------------
// Add Data
//----------------------------------------------------------------------
template <class DataType>
bool				
CTypeMap<DataType>::AddData( unsigned int id, DataType* pData )
{
	typename TYPE_MAP::iterator iData = this->find(id);

	if (iData != this->end())
	{
		
		
		return false;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	this->insert(typename TYPE_MAP::value_type(id, pData));

	return true;
}

//----------------------------------------------------------------------
// Get Data
//----------------------------------------------------------------------
template <class DataType>
DataType*	
CTypeMap<DataType>::GetData( unsigned int id )
{
	typename TYPE_MAP::iterator iData = this->find(id);

	if (iData == this->end())
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
CTypeMap<DataType>::RemoveData( unsigned int id )
{
	typename TYPE_MAP::iterator iData = this->find(id);

	if (iData == this->end())
	{
		
		return false;
	}

	
	delete (*iData).second;

	this->erase( iData );

	return true;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
template <class DataType>
void
CTypeMap<DataType>::SaveToFile(std::ofstream& file)
{
	typename TYPE_MAP::iterator iData = this->begin();

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	int infoSize = this->size();
	file.write((const char*)&infoSize, 4);

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	while (iData != this->end())
	{
		unsigned int	id		= (*iData).first;
		DataType*		pData	= (*iData).second;		

		file.write((const char*)&id, 4);	
		pData->SaveToFile( file );			

		iData ++;
	}
}
		
//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
template <class DataType>
void
CTypeMap<DataType>::LoadFromFile(std::ifstream& file)
{
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	Release();

	//-----------------------------------------------------
	// size 
	//-----------------------------------------------------
	int infoSize;
	file.read((char*)&infoSize, 4);

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	unsigned int id;
	for (int i=0; i<infoSize; i++)
	{		
		file.read((char*)&id, 4);
		DataType*	pData = new DataType;

		pData->LoadFromFile( file );

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (!AddData( id, pData ))
		{
			delete pData;
		}
	}
}



#endif

