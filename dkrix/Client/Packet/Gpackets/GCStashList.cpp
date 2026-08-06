//////////////////////////////////////////////////////////////////////////////
// Filename    : GCStashList.cpp 

// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCStashList.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Item.h"
	#include "ItemInfoManager.h"
	#include "Inventory.h"
	#include "AR.h"
	#include "SR.h"
	#include "SG.h"
	#include "SMG.h"
	#include "Belt.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GCStashList::GCStashList() 
	throw ()
{
	__BEGIN_TRY
 
	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			m_bExist[r][i]       = false;
			m_SubItemsCount[r][i] = 0;
		}
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCStashList::~GCStashList() 
	throw ()
{
	__BEGIN_TRY
	
	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			std::list<SubItemInfo*>::iterator itr = m_pSubItems[r][i].begin();
			for (; itr != m_pSubItems[r][i].end(); itr++)
			{
				SubItemInfo* pItemInfo = *itr;
				delete pItemInfo;
			}
		}
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCStashList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE i      = 0;
	BYTE nTotal = 0;
	BYTE rack   = 0;
	BYTE index  = 0;
	
	
	iStream.read(m_StashNum);

	
	iStream.read(nTotal);

	
	for (i=0; i<nTotal; i++)
	{
		
		iStream.read(rack);
		iStream.read(index);
		_STASHITEM& item = m_pItems[rack][index];

		iStream.read(item.objectID);
		iStream.read(item.itemClass);
		iStream.read(item.itemType);

		BYTE optionSize;
		iStream.read(optionSize);
		for (int j=0; j<optionSize; j++)
		{
			OptionType_t optionType;
			iStream.read(optionType);
			item.optionType.push_back( optionType );
		}

		iStream.read(item.durability);
		iStream.read(item.num);
		iStream.read(item.silver);
		iStream.read(item.grade);
		iStream.read(item.enchantLevel);
		
		
		iStream.read( m_SubItemsCount[rack][index] );
		for (int s=0; s < m_SubItemsCount[rack][index]; s++)
		{
			SubItemInfo* pSubItemInfo = new SubItemInfo();
			pSubItemInfo->read(iStream);
			m_pSubItems[rack][index].push_back(pSubItemInfo);
		}
		
		m_bExist[rack][index] = true;
	}

	
	iStream.read(m_StashGold);

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCStashList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	BYTE r = 0;
	BYTE i = 0;
	BYTE nTotal = 0;
	
	
	for (r=0; r<STASH_RACK_MAX; r++)
		for (i=0; i<STASH_INDEX_MAX; i++)
			if (m_bExist[r][i]) nTotal++;
		
	
	oStream.write(m_StashNum);

	
	oStream.write(nTotal);

	// write each item info
	for (r=0; r<STASH_RACK_MAX; r++)
	{
		for (i=0; i<STASH_INDEX_MAX; i++)
		{
			if (m_bExist[r][i])
			{
				const _STASHITEM& item = m_pItems[r][i];
				oStream.write(r);
				oStream.write(i);
				oStream.write(item.objectID);
				oStream.write(item.itemClass);
				oStream.write(item.itemType);

				BYTE optionSize = item.optionType.size();
				oStream.write(optionSize);
				std::list<OptionType_t>::const_iterator iOption;
				for (iOption=item.optionType.begin(); iOption!=item.optionType.end(); iOption++)
				{
					OptionType_t optionType = *iOption;
					oStream.write(optionType);
				}

				oStream.write(item.durability);
				oStream.write(item.num);
				oStream.write(item.silver);
				oStream.write(item.grade);
				oStream.write(item.enchantLevel);

				
				oStream.write(m_SubItemsCount[r][i]);

				std::list<SubItemInfo*>::const_iterator itr = m_pSubItems[r][i].begin();
				for (; itr != m_pSubItems[r][i].end(); itr++)
				{
					if (*itr) (*itr)->write(oStream);
				}
			}
		}
	}

	
	oStream.write(m_StashGold);
	
	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCStashList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCStashListHandler::execute( this , pPlayer );

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PacketSize_t GCStashList::getPacketSize () const 
	throw ()
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE; 

	size += szBYTE; 

	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			if (m_bExist[r][i]) 
			{
				
				size += szBYTE*2; 

				
				/*
				size += szObjectID;
				size += szBYTE;
				size += szItemType;
				size += szBYTE + optionType.size();
				size += szDurability;
				size += szItemNum;
				size += szSilver;
				size += szGrade;
				size += szEnchantLevel;
				*/
				size += m_pItems[r][i].getPacketSize();

				
				size += szBYTE;

				
				size += SubItemInfo::getSize() * m_SubItemsCount[r][i];
			}
		}
	}

	size += szGold; 

	return size;

	__END_DEBUG
	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__	
//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
std::string GCStashList::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCStashList(";
	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			msg << "(Item:" << r << ", " << i << ":";
			if (m_bExist[r][i])
			{
				msg << "ObjectID:"     << (int)(m_pItems[r][i].objectID)
					<< "ItemClass:"    << (int)(m_pItems[r][i].itemClass) 
					<< "ItemType:"     << (int)(m_pItems[r][i].itemType)
					<< "OptionTypeSize:"   << (int)(m_pItems[r][i].optionType.size())
					<< "Durability:"   << (int)(m_pItems[r][i].durability)
					<< "Num:"          << (int)(m_pItems[r][i].num)
					<< "Silver:"       << (int)(m_pItems[r][i].silver)
					<< "Grade:"        << (int)(m_pItems[r][i].grade)
					<< "EnchantLevel:" << (int)(m_pItems[r][i].enchantLevel);
			}
			msg << ")";
		}
	}
	msg << ")";
	return msg.toString();
		
	__END_CATCH
}
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool GCStashList::isExist(BYTE rack, BYTE index) const
	throw()
{
	__BEGIN_TRY

	Assert(rack < STASH_RACK_MAX && index < STASH_INDEX_MAX);
	return m_bExist[rack][index];

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
STASHITEM GCStashList::getStashItem(BYTE rack, BYTE index) const
	throw()
{
	__BEGIN_TRY

	Assert(rack < STASH_RACK_MAX && index < STASH_INDEX_MAX);
	return m_pItems[rack][index];

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void GCStashList::setStashItem(BYTE rack, BYTE index, Item* pItem) 
	throw()
{
	__BEGIN_TRY

#ifdef	__GAME_SERVER__

	Assert(rack < STASH_RACK_MAX && index < STASH_INDEX_MAX);
	Assert(pItem != NULL);	

	ItemInfo*  pItemInfo      = NULL;
	AR*        pAR            = NULL;
	SR*        pSR            = NULL;
	SG*        pSG            = NULL;
	SMG*       pSMG           = NULL;
	Belt*      pBelt          = NULL;
	BYTE       pocketCount    = 0;
	Inventory* pBeltInventory = NULL;
	BYTE       subItemCount   = 0;
	BYTE       i              = 0;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_AR:
			pAR = dynamic_cast<AR*>(pItem);
			m_pItems[rack][index].objectID     = pItem->getObjectID();
			m_pItems[rack][index].itemClass    = pItem->getItemClass();
			m_pItems[rack][index].itemType     = pItem->getItemType();
			m_pItems[rack][index].optionType   = pItem->getOptionTypeList();
			m_pItems[rack][index].durability   = pItem->getDurability();
			m_pItems[rack][index].num          = pAR->getBulletCount();
			m_pItems[rack][index].silver       = pItem->getSilver();
			m_pItems[rack][index].Grade        = pItem->getGrade();
			m_pItems[rack][index].enchantLevel = pItem->getEnchantLevel();
			break;
		case Item::ITEM_CLASS_SR:
			pSR = dynamic_cast<SR*>(pItem);
			m_pItems[rack][index].objectID     = pItem->getObjectID();
			m_pItems[rack][index].itemClass    = pItem->getItemClass();
			m_pItems[rack][index].itemType     = pItem->getItemType();
			m_pItems[rack][index].optionType   = pItem->getOptionTypeList();
			m_pItems[rack][index].durability   = pItem->getDurability();
			m_pItems[rack][index].num          = pSR->getBulletCount();
			m_pItems[rack][index].silver       = pItem->getSilver();
			m_pItems[rack][index].grade        = pItem->getGrade();
			m_pItems[rack][index].enchantLevel = pItem->getEnchantLevel();
			break;
		case Item::ITEM_CLASS_SG:
			pSG = dynamic_cast<SG*>(pItem);
			m_pItems[rack][index].objectID     = pItem->getObjectID();
			m_pItems[rack][index].itemClass    = pItem->getItemClass();
			m_pItems[rack][index].itemType     = pItem->getItemType();
			m_pItems[rack][index].optionType   = pItem->getOptionTypeList();
			m_pItems[rack][index].durability   = pItem->getDurability();
			m_pItems[rack][index].num          = pSG->getBulletCount();
			m_pItems[rack][index].silver       = pItem->getSilver();
			m_pItems[rack][index].grade        = pItem->getGrade();
			m_pItems[rack][index].enchantLevel = pItem->getEnchantLevel();
			break;
		case Item::ITEM_CLASS_SMG:
			pSMG = dynamic_cast<SMG*>(pItem);
			m_pItems[rack][index].objectID     = pItem->getObjectID();
			m_pItems[rack][index].itemClass    = pItem->getItemClass();
			m_pItems[rack][index].itemType     = pItem->getItemType();
			m_pItems[rack][index].optionType   = pItem->getOptionTypeList();
			m_pItems[rack][index].durability   = pItem->getDurability();
			m_pItems[rack][index].num          = pSMG->getBulletCount();
			m_pItems[rack][index].silver       = pItem->getSilver();
			m_pItems[rack][index].Grade        = pItem->getGrade();
			m_pItems[rack][index].enchantLevel = pItem->getEnchantLevel();
			break;
		case Item::ITEM_CLASS_BELT:
			m_pItems[rack][index].objectID     = pItem->getObjectID();
			m_pItems[rack][index].itemClass    = pItem->getItemClass();
			m_pItems[rack][index].itemType     = pItem->getItemType();
			m_pItems[rack][index].optionType   = pItem->getOptionTypeList();
			m_pItems[rack][index].durability   = pItem->getDurability();
			m_pItems[rack][index].num          = pItem->getNum();
			m_pItems[rack][index].Grade	       = pItem->getGrade();
			m_pItems[rack][index].enchantLevel = pItem->getEnchantLevel();

			
			
			pBelt          = dynamic_cast<Belt*>(pItem);
			pItemInfo      = g_pItemInfoManager->getItemInfo(pBelt->getItemClass(), pBelt->getItemType());
			pocketCount    = dynamic_cast<BeltInfo*>(pItemInfo)->getPocketCount();
			pBeltInventory = pBelt->getInventory();

			for (i = 0; i<pocketCount; i++)
			{
				Item* pBeltItem = pBeltInventory->getItem((int)i, 0);
				
				if (pBeltItem != NULL)
				{
					SubItemInfo* pSubItemInfo = new SubItemInfo;
					Assert(pSubItemInfo != NULL);

					pSubItemInfo->setObjectID(pBeltItem->getObjectID());
					pSubItemInfo->setItemClass(pBeltItem->getItemClass());
					pSubItemInfo->setItemType(pBeltItem->getItemType());
					pSubItemInfo->setItemNum(pBeltItem->getNum());
					pSubItemInfo->setSlotID(i);

					
					m_pSubItems[rack][index].push_back(pSubItemInfo);

					
					subItemCount++;
				}
			}
			
			m_SubItemsCount[rack][index] = subItemCount;

			break;
		default:
			m_pItems[rack][index].objectID     = pItem->getObjectID();
			m_pItems[rack][index].itemClass    = pItem->getItemClass();
			m_pItems[rack][index].itemType     = pItem->getItemType();
			m_pItems[rack][index].optionType   = pItem->getOptionTypeList();
			m_pItems[rack][index].durability   = pItem->getDurability();
			m_pItems[rack][index].num          = pItem->getNum();
			m_pItems[rack][index].Grade        = pItem->getGrade();
			m_pItems[rack][index].enchantLevel = pItem->getEnchantLevel();
			break;
	}

	m_bExist[rack][index] = true;

#endif

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::list<SubItemInfo*>& GCStashList::getSubItems(BYTE rack, BYTE index)
	throw()
{
	__BEGIN_TRY

	Assert(rack < STASH_RACK_MAX && index < STASH_INDEX_MAX);
	return m_pSubItems[rack][index];

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
BYTE GCStashList::getSubItemCount(BYTE rack, BYTE index)
	throw()
{
	__BEGIN_TRY

	Assert(rack < STASH_RACK_MAX && index < STASH_INDEX_MAX);
	return m_SubItemsCount[rack][index];

	__END_CATCH
}


