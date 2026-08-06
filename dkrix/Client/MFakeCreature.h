//----------------------------------------------------------------------
// MFakeCreature.h
//----------------------------------------------------------------------

//
//
//----------------------------------------------------------------------

#ifndef __MFAKECREATURE_H__
#define	__MFAKECREATURE_H__

#include "MCreatureWear.h"
#include <list>
#include <queue>
#include "MemoryPool.h"

#ifndef __DNODE_DEFINE__
#define __DNODE_DEFINE__
//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class DNode {
	public :
		DNode(int _x=0, int _y=0, int _d=0, int _ds=0, int _s=0)
		{ 
			x=_x;
			y=_y;
			direction=_d;
			distance=_ds;
			step = _s;
			pParent=NULL;
		}

		void	SetParent(DNode* pDNode)	{ pParent = pDNode; }

		int		x;
		int		y;
		int		direction;
		int		distance;
		int		step;
		DNode*	pParent;
};


//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------



class Comparison {
	public :
		
		
		
		bool operator () (DNode * left, DNode * right) const
		{ 
			int diff = left->distance - right->distance;			
			int s = left->step - right->step;

			
			if (diff==0)
			{			
				
				if (s==0)
				{
					if (left->pParent!=NULL && right->pParent!=NULL)
					{
						if (right->pParent->direction == right->direction)
						{
							return true;	
						}
						
						return false;	
					}
				}
				
				else if (s>0) 
				{
					return true;
				}
				
				return false;				
			}			
			
			else if (diff>0) return true; 

			return false;
		}
};
#endif


class MFakeCreature : public MCreatureWear {
	typedef	std::list<BYTE>		DIRECTION_LIST;
	
	public :
		enum FAKE_CREATURE_TYPE
		{
			FAKE_CREATURE_STAND,			
			FAKE_CREATURE_WANDER,			
			FAKE_CREATURE_FADE_ACTION,		
			FAKE_CREATURE_TO_PORTAL,		
			FAKE_CREATURE_TURNING_MOVE,		
			FAKE_CREATURE_FAST_MOVE_ACTION,	
			FAKE_CREATURE_BRIGHTNESS,		
			FAKE_CREATURE_TRACE,			
			FAKE_CREATURE_GHOST,			
			FAKE_CREATURE_ROCKET,			
			FAKE_CREATURE_WILD_WOLF,		
			FAKE_CREATURE_DRAGON_TORNADO,	
		};

		enum WILD_WOLF_ATTACK_MODE 
		{
			 WILD_WOLF_MODE_ATTACK_1st = 0, 
			 WILD_WOLF_MODE_MOVE_1st,		
			 WILD_WOLF_MODE_ATTACK_2nd,		
			 WILD_WOLF_MODE_MOVE_2st,		
			 WILD_WOLF_MODE_ATTACK_3rd,		
			 WILD_WOLF_MODE_REMOVE,			
		};
	public :
		MFakeCreature();
		virtual ~MFakeCreature();

		void* operator new( size_t size )
		{
			return g_FakeCreatureMemoryPool.Alloc();
		}

		void operator delete( void* pmem )
		{
			g_FakeCreatureMemoryPool.Free( pmem );
		}

		//----------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------
		
		bool	SetNextDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);
		bool	SetDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);		
		bool	IsNextDirectionNULL()		{ return m_listDirection.empty(); }
		void	SetStop();		
		
		// get
		void	GetDestination(POINT &dest);
		void	GetNextDestination(POINT &dest);
		bool	GetNextPosition(POINT &next);
		BYTE	GetNextDirection()	const			{ return m_listDirection.front(); }

		
		void	RemoveNodes();

		
		int		CalculateDistance(int x, int y);

		//------------------------------------------------------
		// class type
		//------------------------------------------------------
		virtual CLASS_TYPE	GetClassType() const	{ return CLASS_FAKE; }

		//------------------------------------------------------
		// Fake CreatureType
		//------------------------------------------------------
		FAKE_CREATURE_TYPE	GetFakeCreatureType() const		{ return m_FakeCreatureType; }
		void				SetFakeCreatureType(FAKE_CREATURE_TYPE fct);

		//------------------------------------------------------
		// Set FakePosition
		//------------------------------------------------------
		void				SetFakePosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);
		TYPE_SECTORPOSITION	GetFakeX() const	{ return m_FakeX; }
		TYPE_SECTORPOSITION	GetFakeY() const	{ return m_FakeY; }

		//------------------------------------------------------
		// Action
		//------------------------------------------------------
		virtual void		Action();
		virtual void		ActionMove();
		void				ActionMovePet();
		void				KeepTraceCreature();

		void				UpdateFake();
		void				SetDelay(DWORD			Time = 0) { m_Delay = Time; }
		
		bool				IsFakeEnd();

		void				SetOwnerID(TYPE_OBJECTID id);
		const TYPE_OBJECTID	GetOwnerID() const					{ return m_OwnerID; }

		static DWORD		GetFakeID();

		MPetItem*			GetPetItem()					{ return m_pPetItem; }
		void				SetPetItem(MPetItem* pItem)		{ m_pPetItem = pItem; }

		
		//void				SetActionResult(MActionResult* pResult);

		
		void				PacketSpecialActionToOther(TYPE_ACTIONINFO nActionInfo, TYPE_OBJECTID id, MActionResult* pActionResult);
		void				SetFakeCreatureFastMoveAction(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY,
											 TYPE_ACTIONINFO skillID, 
											 TYPE_OBJECTID targetID); 
											 //MActionResult* pResult);
		void				SetZone(MZone* pZone);
		static POINT		CaculatePetPosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, int distance, int distanceMin, MOVE_TYPE moveType, TYPE_SECTORPOSITION petX, TYPE_SECTORPOSITION petY);
		void				SyncTurretDirection();
		const BYTE			GetTurretDirection() const;
		void				SetTurretFinalDirection(BYTE d);
		void				UpdateTurret();
		void				SetTurretDelay( BYTE delay ) { m_TurretDelay = delay; }
		const BYTE&			GetTurretDelay() const		{ return m_TurretDelay; }
		
		//2004, 03, 22 sobeit add start
		void				KeepWanderCreature();
		bool				IsInSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_SECTORPOSITION eX, TYPE_SECTORPOSITION eY);
		//2004, 03, 22 sobeit add end

		// 2004, 11, 8, sobeit add start
		void KeepRocketLuncher();
		void SetTurreDirection(BYTE Dir) { m_TurretDirect = Dir;}
		void SetTime(DWORD dwTime) { m_nextMoveTime = dwTime; }
		// 2004, 11, 8, sobeit add end
		
		// 2004, 12, 31, sobeit add start
		void KeepWildWolfAttack();
		void SetIsEatCorpse(bool IsEat) { m_IsEatCorps = IsEat;	}
		void SetTargetTile(WORD x, WORD y) { m_DestX = x; m_DestY = y;}
		void SetFakeMoveOnce(BYTE Dir);
		// 2004, 12, 31, sobeit add end
		
	protected :
		FAKE_CREATURE_TYPE		m_FakeCreatureType;

		
		TYPE_SECTORPOSITION		m_FakeX;
		TYPE_SECTORPOSITION		m_FakeY;
		float					m_FakeSX;
		float					m_FakeSY;
		float					m_FakeCX;
		float					m_FakeCY;
		DWORD					m_Delay;
		
		TYPE_OBJECTID			m_OwnerID;		
		bool					m_bTraceFlag;
		
		MPetItem*				m_pPetItem;

		static DWORD			m_FakeID;		

		DIRECTION_LIST	m_listDirection;

		TYPE_SECTORPOSITION			m_DestX;
		TYPE_SECTORPOSITION			m_DestY;					

		TYPE_SECTORPOSITION			m_NextDestX; 
		TYPE_SECTORPOSITION			m_NextDestY;				

		TYPE_SECTORPOSITION			m_BlockDestX;				
		TYPE_SECTORPOSITION			m_BlockDestY;				// 

		
		typedef std::priority_queue<DNode*, std::vector<DNode*>, Comparison>	DNODE_PQ;
		typedef	std::list<DNode*>												DNODE_LIST;

		DNODE_PQ			m_pqDNodes;
		DNODE_LIST			m_listDNodes;

		
		DWORD						m_nextMoveTime;
		int							m_PatrolCount;
		BYTE						m_TurretDirect;
		BYTE						m_FinalTurretDirect;
		BYTE						m_TurretDelay;

		
		BYTE						m_WildWolf_Mode;
		bool						m_IsEatCorps; 

};


#endif


