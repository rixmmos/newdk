//----------------------------------------------------------------------
// MSector.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)

#include "MObject.h"
#include "MItem.h"
#include "MCreature.h"
#include "MImageObject.h"
#include "MEffect.h"
#include "MSector.h"
// Note: MTopView.h removed - MSector now uses EffectSpriteTypeHelper (decoupled for demo/game flexibility)
#include "EffectSpriteTypeDef.h"
#include "EffectSpriteTypeHelper.h"
#include <fstream>

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
MSector::MSector(TYPE_SPRITEID	spriteID)
{
	m_SpriteID = spriteID;

	m_fProperty		= 0;

	
	m_nImageObject	= 0;

	//m_FilterSpriteID = SPRITEID_NULL;

	m_Light = 0;

	m_fProperty2 = 0;

	m_bDarkness = false;
	m_bSanctuary = false;	
	m_bGroundElemental = false;	
	m_bDarknessForbidden = false;	

	m_nGroundCreature = 0;
	m_nUndergroundCreature = 0;
	m_nFlyingCreature = 0;
	m_bVisitedFlag = false;
	m_bFuryOfGnome = false;
	m_bDisableTileImage = false;
}

MSector::~MSector()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
void	
MSector::Set(TYPE_SPRITEID spriteID, BYTE fObject)
{ 
	m_SpriteID	= spriteID;
	m_fProperty	= fObject;

	// m_mapObject.clear();
}

//----------------------------------------------------------------------
// Remove All Object
//----------------------------------------------------------------------
void	
MSector::RemoveAllObject()	
{ 
	m_mapObject.clear(); 
	m_nImageObject;
	m_fProperty = 0; 
	m_fProperty2 = 0;

	m_nGroundCreature = 0;
	m_nUndergroundCreature = 0;
	m_nFlyingCreature = 0;
}

//----------------------------------------------------------------------
// Save to File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MSector::SaveToFile(ofstream& file)
{
	BYTE flag = 0;

	
	if (m_fProperty & FLAG_SECTOR_BLOCK_UNDERGROUND)
	{
		
		flag |= FLAG_SECTOR_BLOCK_UNDERGROUND;
	}

	
	if (m_fProperty & FLAG_SECTOR_BLOCK_GROUND)
	{
		
		flag |= FLAG_SECTOR_BLOCK_GROUND;
	}

	
	if (m_fProperty & FLAG_SECTOR_BLOCK_FLYING)
	{
		
		flag |= FLAG_SECTOR_BLOCK_FLYING;
	}

	
	if (m_fProperty & FLAG_SECTOR_PORTAL)
	{
		
		flag |= FLAG_SECTOR_PORTAL;
	}
	

	file.write((const char*)&m_SpriteID, SIZE_SPRITEID);
	file.write((const char*)&flag, 1);
	file.write((const char*)&m_Light, 1);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
MSector::LoadFromFile(ifstream& file)
{
	//----------------------------------------
	
	
	
	
	//----------------------------------------
	// m_listObject.clear();

	file.read((char*)&m_SpriteID, SIZE_SPRITEID);
	file.read((char*)&m_fProperty, 1);
	file.read((char*)&m_Light, 1);
}


//----------------------------------------------------------------------
// Assign Operator
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void	
MSector::operator = (const MSector& sector)
{
	m_SpriteID = sector.m_SpriteID;

	// undeground block
	if (sector.m_fProperty & FLAG_SECTOR_BLOCK_UNDERGROUND)
		m_fProperty |= FLAG_SECTOR_BLOCK_UNDERGROUND;

	// ground block
	if (sector.m_fProperty & FLAG_SECTOR_BLOCK_GROUND)
		m_fProperty |= FLAG_SECTOR_BLOCK_GROUND;

	// flying block
	if (sector.m_fProperty & FLAG_SECTOR_BLOCK_FLYING)
		m_fProperty |= FLAG_SECTOR_BLOCK_FLYING;
}

//----------------------------------------------------------------------
// Can Stand UndergroundCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool
MSector::CanStandUndergroundCreature() const
{
	if (m_fProperty & FLAG_SECTOR_BLOCK_UNDERGROUND		
		|| m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE
		|| m_fProperty2 & FLAG_SECTOR_BLOCK_SERVER_UNDERGROUND)
		return false;

	return true;
}

//----------------------------------------------------------------------
// Can Stand Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool
MSector::CanStandGroundCreature() const
{
	if (m_fProperty & FLAG_SECTOR_BLOCK_GROUND	
		|| m_fProperty & FLAG_SECTOR_GROUNDCREATURE
		|| m_fProperty2 & FLAG_SECTOR_BLOCK_SERVER_GROUND)
		return false;

	return true;
}


//----------------------------------------------------------------------
// Can Stand FlyingCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool
MSector::CanStandFlyingCreature() const
{
	if (m_fProperty & FLAG_SECTOR_BLOCK_FLYING
		|| m_fProperty & FLAG_SECTOR_FLYINGCREATURE
		|| m_fProperty2 & FLAG_SECTOR_BLOCK_SERVER_FLYING)		
		return false;

	return true;
}



//----------------------------------------------------------------------
// 
//                        Add Object
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool	
MSector::AddItem(const MItem* pItem)
{
	
	if (m_fProperty & FLAG_SECTOR_ITEM)
		return false;

	
	m_mapObject.insert( OBJECT_MAP::value_type(POSITION_ITEM, (MObject*)pItem) );

	
	m_fProperty |= FLAG_SECTOR_ITEM;
	if( pItem->GetItemClass() == ITEM_CLASS_CORPSE )
	{
		const MCorpse* pCorpse = dynamic_cast<const MCorpse*>(pItem);
		if( pCorpse == NULL || pCorpse->GetCreature() == NULL )
			return true;

		const MCreature* pCreature = pCorpse->GetCreature();

		if( pCreature->GetCreatureType() == 672 )
			m_fProperty |= FLAG_SECTOR_BLOCK_GROUND;
	}

	return true;
}

//----------------------------------------------------------------------
// Add UndergroundCreature
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool	
MSector::AddUndergroundCreature(const MCreature* pUndergroundCreature)
{
	
	//if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
	//	return false;

	BYTE pos = POSITION_UNDERGROUNDCREATURE+m_nUndergroundCreature;

	if (pos > POSITION_UNDERGROUNDCREATURE_MAX)
		return false;
	

	int beforeSize = m_mapObject.size();

	
	m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pUndergroundCreature) );

	int afterSize = m_mapObject.size();

	
	if (afterSize==beforeSize)
	{
		BYTE previousPos = POSITION_UNDERGROUNDCREATURE-1;

		
		OBJECT_MAP::const_iterator iCreature = GetUndergroundCreatureIterator();
		BYTE currentPos;
		int i;

		for (i=0; i<beforeSize; i++)
		{
			currentPos = iCreature->first;

			
			
			if (currentPos - previousPos > 1)
			{
				pos = currentPos - 1;
				m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pUndergroundCreature) );

				break;
			}

			previousPos = currentPos;

			iCreature ++;
		}

		
		if (i==beforeSize)
		{
			pos = currentPos + 1;
			m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pUndergroundCreature) );
		}
	}

	
	m_fProperty |= FLAG_SECTOR_UNDERGROUNDCREATURE;

	m_nUndergroundCreature ++;

	return true;
}

//----------------------------------------------------------------------
// Add Creature
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool	
MSector::AddGroundCreature(const MCreature* pCreature)
{
	
	//if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
	//	return false;

	//----------------------------------------------------------
	
	//----------------------------------------------------------
	 

	BYTE pos = POSITION_GROUNDCREATURE+m_nGroundCreature;

	if (pos > POSITION_GROUNDCREATURE_MAX)
		return false;

	int beforeSize = m_mapObject.size();

	
	m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pCreature) );

	int afterSize = m_mapObject.size();

	
	if (afterSize==beforeSize)
	{
		BYTE previousPos = POSITION_GROUNDCREATURE-1;

		
		OBJECT_MAP::const_iterator iCreature = GetGroundCreatureIterator();
		BYTE currentPos;
		int i;

		for (i=0; i<beforeSize; i++)
		{
			currentPos = iCreature->first;

			
			
			if (currentPos - previousPos > 1)
			{
				pos = currentPos - 1;
				m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pCreature) );

				break;
			}

			previousPos = currentPos;

			iCreature ++;
		}

		
		if (i==beforeSize)
		{
			pos = currentPos + 1;
			m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pCreature) );
		}
	}


	
	m_fProperty |= FLAG_SECTOR_GROUNDCREATURE;

	m_nGroundCreature ++;

	return true;
}

//----------------------------------------------------------------------
// Add FlyingCreature
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool	
MSector::AddFlyingCreature(const MCreature* pFlyingCreature)
{
	
	//if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
	//	return false;

	BYTE pos = POSITION_FLYINGCREATURE+m_nFlyingCreature;

	if (pos > POSITION_FLYINGCREATURE_MAX)
		return false;	

	int beforeSize = m_mapObject.size();

	
	m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pFlyingCreature) );

	int afterSize = m_mapObject.size();

	
	if (afterSize==beforeSize)
	{
		BYTE previousPos = POSITION_FLYINGCREATURE-1;

		
		OBJECT_MAP::const_iterator iCreature = GetFlyingCreatureIterator();
		BYTE currentPos;
		int i;

		for (i=0; i<beforeSize; i++)
		{
			currentPos = iCreature->first;

			
			
			if (currentPos - previousPos > 1)
			{
				pos = currentPos - 1;
				m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pFlyingCreature) );

				break;
			}

			previousPos = currentPos;

			iCreature ++;
		}

		
		if (i==beforeSize)
		{
			pos = currentPos + 1;
			m_mapObject.insert( OBJECT_MAP::value_type(pos, (MObject*)pFlyingCreature) );
		}
	}

	
	m_fProperty |= FLAG_SECTOR_FLYINGCREATURE;

	m_nFlyingCreature ++;

	return true;
}

//----------------------------------------------------------------------
// Add ImageObject
//----------------------------------------------------------------------




//


//
//----------------------------------------------------------------------
bool	
MSector::AddImageObject(const MImageObject* pImageObject)
{
	BYTE pos = POSITION_IMAGEOBJECT+m_nImageObject;

	if (pos >= POSITION_EFFECT)
		return false;

	
	m_mapObject.insert( 
		OBJECT_MAP::value_type(pos, (MObject*)pImageObject) 
	);

	
	m_nImageObject ++;

	return true;
}


//----------------------------------------------------------------------
// 
//                         Get Object
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Get Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem* const			
MSector::GetItem() const
{
	
	if (m_fProperty & FLAG_SECTOR_ITEM)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.find(POSITION_ITEM);

		
		if (iObject == m_mapObject.end()) 
			return NULL;
		
		
		return (MItem* const)((*iObject).second);
	}

	return NULL;
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::const_iterator
MSector::GetCreatureIterator() const
{
	
	if (IsExistAnyCreature())
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get UndergroundCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::const_iterator
MSector::GetUndergroundCreatureIterator() const
{
	
	if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);
		
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::const_iterator
MSector::GetGroundCreatureIterator() const
{
	
	if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_GROUNDCREATURE);

		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_GROUNDCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}
		
//----------------------------------------------------------------------
// Get FlyingCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::const_iterator
MSector::GetFlyingCreatureIterator() const
{
	
	if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_FLYINGCREATURE);

		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::iterator
MSector::GetCreatureIterator()
{
	
	if (IsExistAnyCreature())
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get UndergroundCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::iterator
MSector::GetUndergroundCreatureIterator()
{
	
	if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);
		
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::iterator
MSector::GetGroundCreatureIterator()
{
	
	if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.lower_bound(POSITION_GROUNDCREATURE);

		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_GROUNDCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}
		
//----------------------------------------------------------------------
// Get FlyingCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
OBJECT_MAP::iterator
MSector::GetFlyingCreatureIterator()
{
	
	if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.lower_bound(POSITION_FLYINGCREATURE);

		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			return iObject;
		}
	}

	return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature * const
MSector::GetCreature() const
{
	
	if (IsExistAnyCreature())
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			return (MCreature* const)(iObject->second);
		}
	}
	
	return NULL;
}

//----------------------------------------------------------------------
// Get UndergroundCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature * const
MSector::GetUndergroundCreature() const
{
	
	if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);
		
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX)
		{
			return (MCreature* const)(iObject->second);
		}
	}
	
	return NULL;
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature * const
MSector::GetGroundCreature() const
{
	
	if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_GROUNDCREATURE);
		
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_GROUNDCREATURE_MAX)
		{
			return (MCreature* const)(iObject->second);
		}
	}
	
	return NULL;
}

//----------------------------------------------------------------------
// Get FlyingCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature * const
MSector::GetFlyingCreature() const
{
	
	if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.lower_bound(POSITION_FLYINGCREATURE);
		
		if (iObject!=m_mapObject.end() 
			&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			return (MCreature* const)(iObject->second);
		}
	}
	
	return NULL;
}
	
//----------------------------------------------------------------------
// Get ImageObject
//----------------------------------------------------------------------


//----------------------------------------------------------------------
MImageObject* const	
MSector::GetImageObject(TYPE_OBJECTID id) const
{
	
	if (m_nImageObject)
	{
		
		OBJECT_MAP::const_iterator iObject = m_mapObject.find(POSITION_IMAGEOBJECT);

		
		if (iObject == m_mapObject.end()) 
			return NULL;

		
		
		for (register BYTE i=0; i<m_nImageObject; i++)
		{
			
			if ( ((*iObject).second)->GetID() == id )
				return (MImageObject* const)((*iObject).second);

			
			iObject++;
		}
		
	}

	return NULL;
}


//----------------------------------------------------------------------
//
//                       Remove Object
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Remove Item
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MSector::RemoveItem(MItem*& pItem)
{
	
	if (m_fProperty & FLAG_SECTOR_ITEM)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

		
		if (iObject == m_mapObject.end()) 
			return false;
		
		
		pItem =  (MItem*)((*iObject).second);

		
		m_mapObject.erase( iObject );

		
		if( pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
			((MCorpse*)pItem)->GetCreature() != NULL &&
			((MCorpse*)pItem)->GetCreature()->GetCreatureType() == 672 )
			m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;

		m_fProperty &= ~FLAG_SECTOR_ITEM;

		return true;
	}

	
	return false;
}

//----------------------------------------------------------------------
// Remove UndergroundCreature
//----------------------------------------------------------------------


//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
// Remove Item(id, &Item)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MSector::RemoveItem(TYPE_OBJECTID id, MItem*& pItem)
{
	
	if (m_fProperty & FLAG_SECTOR_ITEM)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

		
		if (iObject == m_mapObject.end()) 
			return false;
		
		pItem =  (MItem*)((*iObject).second);
			
		
		
		
		if (((*iObject).second)->GetID() == id)
		{
			
			m_mapObject.erase( iObject );

			
			if( pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
				((MCorpse*)pItem)->GetCreature() != NULL &&
				((MCorpse*)pItem)->GetCreature()->GetCreatureType() == 672 )
				m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;

			m_fProperty &= ~FLAG_SECTOR_ITEM;

			return true;
		}		
	}

	
	return false;
}

//----------------------------------------------------------------------
// Remove Creature(id, &Creature)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MSector::RemoveCreature(TYPE_OBJECTID id, MCreature*& pCreature)
{
	if (IsExistAnyCreature())
	{
		
		OBJECT_MAP::iterator iObject = GetCreatureIterator();

		while (iObject!=m_mapObject.end() 
				&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			pCreature =  (MCreature*)iObject->second;
				
			
			
			
			if (pCreature->GetID() == id)
			{
				// Underground
				if (iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX)
				{					
					if (--m_nUndergroundCreature==0)
					{
						m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;
					}
				}
				// Ground
				else if (iObject->first <= POSITION_GROUNDCREATURE_MAX)
				{
					if (--m_nGroundCreature==0)
					{
						m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
					}
				}
				// Flying
				else 
				{
					if (--m_nFlyingCreature==0)
					{
						m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
					}
				}		
				
				
				m_mapObject.erase( iObject );				

				return true;
			}

			iObject ++;
		}		
	}

	
	pCreature = NULL;

	return false;
}

//----------------------------------------------------------------------
// Remove UndergroundCreature(id, &UndergroundCreature)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MSector::RemoveUndergroundCreature(TYPE_OBJECTID id, MCreature*& pUndergroundCreature)
{
	
	if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = GetUndergroundCreatureIterator();

		for (int i=0; iObject!=m_mapObject.end() && i<m_nUndergroundCreature; i++)
		{
			pUndergroundCreature =  (MCreature*)iObject->second;
				
			
			
			
			if (pUndergroundCreature->GetID() == id)
			{
				
				m_mapObject.erase( iObject );

				
				if (--m_nUndergroundCreature==0)
				{
					m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;				
				}

				return true;
			}

			iObject ++;
		}		
	}

	
	pUndergroundCreature = NULL;

	return false;
}

//----------------------------------------------------------------------
// Remove Creature(id, &Creature)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MSector::RemoveGroundCreature(TYPE_OBJECTID id, MCreature*& pGroundCreature)
{
	
	if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = GetGroundCreatureIterator();

		for (int i=0; iObject!=m_mapObject.end() && i<m_nGroundCreature; i++)
		{
			pGroundCreature =  (MCreature*)iObject->second;
				
			
			
			
			if (pGroundCreature->GetID() == id)
			{
				
				m_mapObject.erase( iObject );

				
				if (--m_nGroundCreature==0)
				{
					m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
				}

				return true;
			}

			iObject ++;
		}		
	}

	
	pGroundCreature = NULL;

	
	return false;
}
		
//----------------------------------------------------------------------
// Remove FlyingCreature(id, &FlyingCreature)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MSector::RemoveFlyingCreature(TYPE_OBJECTID id, MCreature*& pFlyingCreature)
{
	
	if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = GetFlyingCreatureIterator();

		for (int i=0; iObject!=m_mapObject.end() && i<m_nFlyingCreature; i++)
		{
			pFlyingCreature =  (MCreature*)iObject->second;
				
			
			
			
			if (pFlyingCreature->GetID() == id)
			{
				
				m_mapObject.erase( iObject );

				
				if (--m_nFlyingCreature==0)
				{
					m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
				}

				return true;
			}

			iObject ++;
		}		
	}

	
	pFlyingCreature = NULL;

	
	return false;
}

//----------------------------------------------------------------------
// Remove ImageObject(id, &ImageObject)
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool	
MSector::RemoveImageObject(TYPE_OBJECTID id, MImageObject*& pImageObject)
{
	
	if (m_nImageObject)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_IMAGEOBJECT);

		
		if (iObject == m_mapObject.end()) 
			return false;
		
		
		
		for (register BYTE i=0; i<m_nImageObject; i++)
		{
			
			if (((*iObject).second)->GetID() == id)
			{
				pImageObject =  (MImageObject*)((*iObject).second);

				
				m_mapObject.erase( iObject );

				
				m_nImageObject--;				

				return true;
			}		

			
			iObject++;
		}
	}

	
	return false;
}


//----------------------------------------------------------------------
// Remove Item(id)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MSector::RemoveItem(TYPE_OBJECTID id)
{
	
	if (m_fProperty & FLAG_SECTOR_ITEM)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

		
		if (iObject == m_mapObject.end()) 
			return false;
		
		
		
		
		if (((*iObject).second)->GetID() == id)
		{
			
			MItem* pItem = dynamic_cast<MItem*>(iObject->second);
			if( pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
				((MCorpse*)pItem)->GetCreature() != NULL &&
				((MCorpse*)pItem)->GetCreature()->GetCreatureType() == 672 )
				m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;

			m_mapObject.erase( iObject );

			
			m_fProperty &= ~FLAG_SECTOR_ITEM;

			return true;
		}		
	}

	
	return false;
}


//----------------------------------------------------------------------
// Remove Creature(id)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MSector::RemoveCreature(TYPE_OBJECTID id)
{	
	if (IsExistAnyCreature())
	{
		
		OBJECT_MAP::iterator iObject = GetCreatureIterator();

		while (iObject!=m_mapObject.end() 
				&& iObject->first <= POSITION_FLYINGCREATURE_MAX)
		{
			
			
			
			if (((MCreature*)iObject->second)->GetID() == id)
			{
				// Underground
				if (iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX)
				{
					if (--m_nUndergroundCreature==0)
					{
						m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;
					}					
				}
				// Ground
				else if (iObject->first <= POSITION_GROUNDCREATURE_MAX)
				{
					if (--m_nGroundCreature==0)
					{
						m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
					}
					
				}
				// Flying
				else 
				{
					if (--m_nFlyingCreature==0)
					{
						m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
					}					
				}	
				
				
				m_mapObject.erase( iObject );				

				return true;
			}

			iObject ++;
		}		
	}

	
	return false;
}
		
//----------------------------------------------------------------------
// Remove UndergroundCreature(id)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MSector::RemoveUndergroundCreature(TYPE_OBJECTID id)
{
	
	if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = GetUndergroundCreatureIterator();

		for (int i=0; iObject!=m_mapObject.end() && i<m_nUndergroundCreature; i++)
		{
			
			
			
			if (((MCreature*)iObject->second)->GetID() == id)
			{
				
				m_mapObject.erase( iObject );

				
				if (--m_nUndergroundCreature==0)
				{
					m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;
				}

				return true;
			}

			iObject ++;
		}		
	}

	
	return false;
}
		
//----------------------------------------------------------------------
// Remove Creature(id)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MSector::RemoveGroundCreature(TYPE_OBJECTID id)
{
	
	if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = GetGroundCreatureIterator();

		for (int i=0; iObject!=m_mapObject.end() && i<m_nGroundCreature; i++)
		{
			
			
			
			if (((MCreature*)iObject->second)->GetID() == id)
			{
				
				m_mapObject.erase( iObject );

				
				if (--m_nGroundCreature==0)
				{
					m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
				}				

				return true;
			}

			iObject ++;
		}		
	}

	
	return false;
}
		
//----------------------------------------------------------------------
// Remove FlyingCreature(id)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MSector::RemoveFlyingCreature(TYPE_OBJECTID id)
{
	
	if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
	{
		
		OBJECT_MAP::iterator iObject = GetFlyingCreatureIterator();

		for (int i=0; iObject!=m_mapObject.end() && i<m_nFlyingCreature; i++)
		{
			
			
			
			if (((MCreature*)iObject->second)->GetID() == id)
			{
				
				m_mapObject.erase( iObject );

				
				if (--m_nFlyingCreature==0)
				{
					m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
				}
				

				return true;
			}

			iObject ++;
		}		
	}


	
	return false;
}
		
//----------------------------------------------------------------------
// Remove ImageObject(id)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MSector::RemoveImageObject(TYPE_OBJECTID id)
{
	
	if (m_nImageObject)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_IMAGEOBJECT);

		
		if (iObject == m_mapObject.end()) 
			return false;
		
		
		
		
		for (register BYTE i=0; i<m_nImageObject; i++)
		{
			if (((*iObject).second)->GetID() == id)
			{
				
				m_mapObject.erase( iObject );
				
				m_nImageObject--;				

				return true;
			}		

			
			iObject ++;
		}
	}

	
	return false;
}


//----------------------------------------------------------------------
// Remove Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
MSector::RemoveItem()
{
	
	if (m_fProperty & FLAG_SECTOR_ITEM)
	{
		
		OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

		
		if (iObject == m_mapObject.end()) 
			return false;

		MItem* pItem = dynamic_cast<MItem*>(iObject->second);
		if( pItem->GetItemClass() == ITEM_CLASS_CORPSE && 
			((MCorpse*)pItem)->GetCreature() != NULL &&
			((MCorpse*)pItem)->GetCreature()->GetCreatureType() == 672 )
			m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;
		
		
		m_mapObject.erase( iObject );

		
		m_fProperty &= ~FLAG_SECTOR_ITEM;

		return true;
	}

	
	return false;
}

//----------------------------------------------------------------------
// Remove UndergroundCreature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// ImageObject Iterator
//----------------------------------------------------------------------


//----------------------------------------------------------------------
OBJECT_MAP::const_iterator	
MSector::GetImageObjectIterator() const
{
	if (m_nImageObject)
	{
		
		return m_mapObject.find(POSITION_IMAGEOBJECT);
	}

	return m_mapObject.end();
}


//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Add Effect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MSector::AddEffect(const MEffect* pEffect)
{
	
	if (pEffect!=NULL)
	{
		m_listEffect.push_back((MEffect*)pEffect);

		
		if (!m_bDarkness || !m_bSanctuary)	
		{
			int fid = pEffect->GetFrameID();
			int est = ::GetEffectSpriteType( (BLT_TYPE)pEffect->GetBltType(), fid );

//			if(est >= EFFECTSPRITETYPE_MAP_BLACK_LARGE_SMOKE &&
//				est >= EFFECTSPRITETYPE_MAP_BLACK_SMALL_SMOKE_3)
//				m_bDarkness = true;
			
			if (est>=EFFECTSPRITETYPE_DARKNESS_2_1
					&& est<=EFFECTSPRITETYPE_DARKNESS_2_5)
			{
				m_bDarkness = true;
			}				
			
			if( est>= EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 &&
				est<= EFFECTSPRITETYPE_GRAY_DARKNESS_2_5 )
			{
				m_bDarkness = true;
			}

			
			if (est>=EFFECTSPRITETYPE_SANCTUARY_1
					&& est<=EFFECTSPRITETYPE_SANCTUARY_3)
			{
				m_bSanctuary = true;
			}

		}
	}
}

//----------------------------------------------------------------------
// Get Effect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MEffect*	
MSector::GetEffect(TYPE_OBJECTID id) const
{
	EFFECT_LIST::const_iterator iEffect = m_listEffect.begin();

	while (iEffect != m_listEffect.end())
	{
		
		if ((*iEffect)->GetID()==id)
		{
			return *iEffect;
		}

		iEffect++;
	}

	return NULL;
}


//----------------------------------------------------------------------
// Remove Effect(id)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MSector::RemoveEffect(TYPE_OBJECTID id)
{
	EFFECT_LIST::iterator iEffect = m_listEffect.begin();

	while (iEffect != m_listEffect.end())
	{
		MEffect* pEffect = *iEffect;				

		
		if (pEffect->GetID()==id)
		{
			
			m_listEffect.erase( iEffect );

			
			if (m_bDarkness || m_bSanctuary)	
			{
				int fid = pEffect->GetFrameID();
				int est = ::GetEffectSpriteType( (BLT_TYPE)pEffect->GetBltType(), fid );

//				if(est >= EFFECTSPRITETYPE_MAP_BLACK_LARGE_SMOKE &&
//					est <= EFFECTSPRITETYPE_MAP_BLACK_SMALL_SMOKE_3
//					)
//				{
//					m_bDarkness = false;
//				}
				
				if (est>=EFFECTSPRITETYPE_DARKNESS_2_1
						&& est<=EFFECTSPRITETYPE_DARKNESS_2_5)
				{
					m_bDarkness = false;
				}				

				if (est>=EFFECTSPRITETYPE_GRAY_DARKNESS_2_1
						&& est<=EFFECTSPRITETYPE_GRAY_DARKNESS_2_5)
				{
					m_bDarkness = false;
				}				

				
				if (est>=EFFECTSPRITETYPE_SANCTUARY_1
						&& est<=EFFECTSPRITETYPE_SANCTUARY_3)
				{
					m_bSanctuary = false;
				}
			}

			return true;
		}

		iEffect++;
	}

	return false;
}

//----------------------------------------------------------------------
// Remove Effect(id, &pEffect)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MSector::RemoveEffect(TYPE_OBJECTID id, MEffect*& pEffect)
{
	EFFECT_LIST::iterator iEffect = m_listEffect.begin();

	while (iEffect != m_listEffect.end())
	{
		MEffect* pEffect = *iEffect;

		
		if (pEffect->GetID()==id)
		{
			
			pEffect = *iEffect;

			
			m_listEffect.erase( iEffect );

			
			if (m_bDarkness || m_bSanctuary)	
			{
				int fid = pEffect->GetFrameID();
				int est = ::GetEffectSpriteType( (BLT_TYPE)pEffect->GetBltType(), fid );

				
				if (est>=EFFECTSPRITETYPE_DARKNESS_2_1
						&& est<=EFFECTSPRITETYPE_DARKNESS_2_5)
				{
					m_bDarkness = false;
				}				

				if (est>=EFFECTSPRITETYPE_GRAY_DARKNESS_2_1
						&& est<=EFFECTSPRITETYPE_GRAY_DARKNESS_2_5)
				{
					m_bDarkness = false;
				}				

				
				if (est>=EFFECTSPRITETYPE_SANCTUARY_1
						&& est<=EFFECTSPRITETYPE_SANCTUARY_3)
				{
					m_bSanctuary = false;
				}
			}

			return true;
		}

		iEffect++;
	}

	return false;
}

//----------------------------------------------------------------------
// Add Portal ( zoneID )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool					
MSector::AddPortal(int type, int zoneID)
{
	PORTAL_LIST::iterator iData = m_listPortal.begin();

	
	
	while (iData != m_listPortal.end())
	{
		
		
		if (iData->ZoneID > zoneID)
		{
			m_listPortal.insert(iData, PORTAL_INFO(type, zoneID));

			m_fProperty |= FLAG_SECTOR_PORTAL;

			return true;
		}

		
		
		if (iData->ZoneID==zoneID)
		{
			return false;
		}

		iData++;
	}	

	
	
	m_listPortal.push_back( PORTAL_INFO(type, zoneID) );

	m_fProperty |= FLAG_SECTOR_PORTAL;

	return true;
}

//----------------------------------------------------------------------
// Clear Portal ( zoneID )
//----------------------------------------------------------------------
void					
MSector::ClearPortal()
{
	m_listPortal.clear();

	m_fProperty &= ~FLAG_SECTOR_PORTAL;
}

//----------------------------------------------------------------------
// Add SectorSound (SECTORSOUND_INFO&)
//----------------------------------------------------------------------
bool								
MSector::AddSectorSound(const SECTORSOUND_INFO& info)
{
	return AddSectorSound( info.ZoneSoundID, info.X, info.Y );
}

//----------------------------------------------------------------------
// Add SectorSound ( ZoneSoundID, Xo, Yo )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool					
MSector::AddSectorSound(int zoneSoundID, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	SECTORSOUND_LIST::iterator iData = m_listSectorSound.begin();

	
	
	while (iData != m_listSectorSound.end())
	{
		
		
		if (iData->ZoneSoundID > zoneSoundID)
		{
			m_listSectorSound.insert(iData, SECTORSOUND_INFO(zoneSoundID, x, y));

			return true;
		}

		
		
		if (iData->ZoneSoundID==zoneSoundID)
		{
			return false;
		}

		iData++;
	}	

	
	
	m_listSectorSound.push_back( SECTORSOUND_INFO(zoneSoundID, x, y) );

	return true;
}

//----------------------------------------------------------------------
// Clear SectorSound ( zoneID )
//----------------------------------------------------------------------
void					
MSector::ClearSectorSound()
{
	m_listSectorSound.clear();
}