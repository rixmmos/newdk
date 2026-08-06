//----------------------------------------------------------------------
// MNPC.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef __MNPC_H__
#define	__MNPC_H__

#include "MCreatureWear.h"
#include "MShop.h"
#include "MemoryPool.h"
class NPC_INFO;

class MNPC : public MCreatureWear {
	public :
		MNPC();
		~MNPC();

		void* operator new( size_t size )
		{
			return g_NPCCreatureMemoryPool.Alloc();
		}

		void operator delete( void* pmem )
		{
			g_NPCCreatureMemoryPool.Free( pmem );
		}

		//------------------------------------------------------
		// class type
		//------------------------------------------------------
		virtual CLASS_TYPE	GetClassType() const	{ return CLASS_NPC; }

		//------------------------------------------------------
		// Action
		//------------------------------------------------------
		virtual void	Action();			

		//------------------------------------------------------
		// NPC ID
		//------------------------------------------------------
		void				SetNPCID(TYPE_OBJECTID id)	{ m_NPCID = id; }
		TYPE_OBJECTID		GetNPCID() const			{ return m_NPCID; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		NPC_INFO*			GetNPCInfo(TYPE_OBJECTID id) const;

		//------------------------------------------------------
		// Shop
		//------------------------------------------------------
		BOOL				HasShop()			{ return m_pShop!=NULL; }
		MShop*				GetShop() const		{ return m_pShop; }
		void				SetShop(MShop* pShop);

		//------------------------------------------------------
		
		//------------------------------------------------------
		BOOL				CreateFixedShelf(bool bMysterious=false);

	protected :
		TYPE_OBJECTID		m_NPCID;		// NPC ID
		MShop*				m_pShop;		
};

#endif


