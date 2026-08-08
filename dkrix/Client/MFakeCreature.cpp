//----------------------------------------------------------------------
// MFakeCreature.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "ClientDef.h"
#include "MFakeCreature.h"
#include "MTopView.h"
#include "DebugInfo.h"
#include "MPlayer.h"
#include "SkillDef.h"
#include "UserInformation.h"
#include "VS_UI.h"
extern DWORD g_CurrentTime;
extern bool			HasEffectStatusSummonSylph( MCreature* pCreature );
extern void			RemoveEffectStatusSummonSylph( MCreature* pCreature );
extern	LONG g_SECTOR_WIDTH;
extern	LONG g_SECTOR_HEIGHT;
extern	LONG g_SECTOR_WIDTH_HALF;
extern	LONG g_SECTOR_HEIGHT_HALF;
extern	LONG g_SECTOR_SKIP_PLAYER_LEFT;
extern	LONG g_SECTOR_SKIP_PLAYER_UP;
extern	LONG g_TILESURFACE_SECTOR_WIDTH;
extern	LONG g_TILESURFACE_SECTOR_HEIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_DOWN;
extern	LONG g_TILESURFACE_WIDTH;
extern	LONG g_TILESURFACE_HEIGHT;
extern	LONG g_TILESURFACE_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_OUTLINE_DOWN;
BYTE GetCreatureActionCountMax( const MCreature* pCreature, int action );

extern POINT GetNextTileByDirection(int TileX, int TileY, BYTE Dir);
#define MoveNextPosition()					\
											\
		MovePosition( m_NextX, m_NextY );	\
		int dir = m_Direction;				\
											\
		SetAction( m_MoveAction );			\
		m_Direction = m_NextDirection;		\
		m_DirectionMove = m_NextDirection;	\
		ActionMove();						\
		m_Direction = dir;					\
											\
		m_NextX = SECTORPOSITION_NULL;		\
		m_NextY = SECTORPOSITION_NULL;		\
		m_NextDirection = DIRECTION_NULL;

#define	ActionMoveNextPosition()					\
		if (!m_bFastMove)							\
		{											\
			if (m_NextX != SECTORPOSITION_NULL		\
				&& m_NextDirection != SECTORPOSITION_NULL)	\
			{										\
				MoveNextPosition();					\
			}										\
			else if (m_NextAction==m_MoveAction)	\
			{										\
				SetAction( m_NextAction );			\
				ActionMove();						\
			}										\
		}
//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
DWORD MFakeCreature::m_FakeID = 0;

//----------------------------------------------------------------------
// Get FakeID
//----------------------------------------------------------------------
DWORD
MFakeCreature::GetFakeID()
{
	
	
	MCreature *pFakeCreature = NULL;

	do
	{
		m_FakeID++;
		if (m_FakeID==10000)
		{
			m_FakeID = 0;
		}
		pFakeCreature = g_pZone->GetFakeCreature(m_FakeID);

	} while(pFakeCreature != NULL);


	return m_FakeID;
}

//----------------------------------------------------------------------
// 
// constructor / destructor
//
//----------------------------------------------------------------------
MFakeCreature::MFakeCreature()
{
	m_FakeCreatureType = FAKE_CREATURE_STAND;

	m_MoveType = CREATURE_FAKE_NO_BLOCK;

	m_FakeX = 0;
	m_FakeY = 0;
	m_Delay = 0;
	m_OwnerID = OBJECTID_NULL;
	m_bTraceFlag = false;

	m_ID = GetFakeID();

	m_pPetItem = NULL;

	
	m_DestX = SECTORPOSITION_NULL;
	m_DestY = SECTORPOSITION_NULL;

	
	m_NextDestX = SECTORPOSITION_NULL;
	m_NextDestY = SECTORPOSITION_NULL;

	
	m_BlockDestX = SECTORPOSITION_NULL;
	m_BlockDestY = SECTORPOSITION_NULL;

	m_nextMoveTime = timeGetTime()+(rand()%8+3)*1000;
	m_PatrolCount = rand()%5+1;

	m_TurretDelay = 0;
	m_WildWolf_Mode = 0;
	m_IsEatCorps = false;
}

MFakeCreature::~MFakeCreature()
{
	if(m_pPetItem != NULL)
	{
		TYPE_OBJECTID id = GetOwnerID();
		if(g_pPlayer != NULL && id != g_pPlayer->GetID())
		{
			delete m_pPetItem;
			m_pPetItem = NULL;
		}
	}

	// remove priority queue 
	RemoveNodes();

	// remove list
	m_listDirection.clear();

}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set Fake Creature
//----------------------------------------------------------------------
void				
MFakeCreature::SetFakeCreatureType(FAKE_CREATURE_TYPE fct)
{ 
	m_FakeCreatureType = fct; 

	switch (fct)
	{
		//----------------------------------------------
		
		//----------------------------------------------
		case FAKE_CREATURE_FAST_MOVE_ACTION :		
		case FAKE_CREATURE_FADE_ACTION :
		case FAKE_CREATURE_BRIGHTNESS :
			SetFade();
		break;

		//----------------------------------------------
		
		//----------------------------------------------
		case FAKE_CREATURE_TO_PORTAL :
			SetCutHeightFrom( 0, 80, 5 );
		break;

		//----------------------------------------------
		
		//----------------------------------------------
		case FAKE_CREATURE_TURNING_MOVE :
			SetFade();
			break;
		case FAKE_CREATURE_ROCKET:
			break;
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
// Action
//----------------------------------------------------------------------
void
MFakeCreature::Action()
{
	if( m_Delay > 0 )
	{
		m_Delay --;
		return;
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (m_NextChatFadeTime < g_CurrentTime)
	{
		FadeChatString();		
	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	UpdateStatus();	
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	UpdateInvisible();
	UpdateCasket();
	UpdateCutHeight();
	UpdateTurning();

	//--------------------------------------------------------
	// Fake
	//--------------------------------------------------------
	UpdateFake();

	UpdateGhost();
	
	switch(GetFakeCreatureType())
	{
		case FAKE_CREATURE_TRACE:
			KeepTraceCreature();
			break;
		case FAKE_CREATURE_GHOST:
			KeepWanderCreature();
			break;
		case FAKE_CREATURE_ROCKET:
			KeepRocketLuncher();
			break;
		case FAKE_CREATURE_WILD_WOLF:
			KeepWildWolfAttack();
			break;

	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	ChangeNearDirection();
	//m_CurrentDirection = m_Direction;

	
	BOOL bSlayer = IsSlayer();

	if (m_nNextUsedActionInfo!=ACTIONINFO_NULL)
	{		
		
		
		TYPE_ACTIONINFO nextUsedActionInfo = m_nNextUsedActionInfo;
		TYPE_ACTIONINFO nextNextUsedActionInfo = ACTIONINFO_NULL;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		{	
			m_nNextUsedActionInfo = ACTIONINFO_NULL;

			
			if (GetActionInfoCastingStartFrame(m_nUsedActionInfo) >= m_ActionCount)
			{
				AttachCastingEffect( m_nUsedActionInfo, TRUE );
			}

			AffectUsedActionInfo( m_nUsedActionInfo );
			m_nUsedActionInfo	= ACTIONINFO_NULL;

			nextNextUsedActionInfo = m_nNextUsedActionInfo;
		}

		m_nUsedActionInfo = nextUsedActionInfo;
		m_nNextUsedActionInfo = nextNextUsedActionInfo;

		int actionInfoAction = GetActionInfoAction(m_nUsedActionInfo);

		
		if (actionInfoAction==ACTION_DAMAGED)
		{
			if //(m_Action!=ACTION_STAND 
				//&& (!IsSlayer() || IsSlayer && m_Action!=ACTION_SLAYER_MOTOR_STAND)
				//&& 
				(GetActionInfoCastingStartFrame(m_nUsedActionInfo) < m_ActionCount)
			{
				AttachCastingEffect( m_nUsedActionInfo, TRUE );

				AffectUsedActionInfo( m_nUsedActionInfo );
		
				m_nUsedActionInfo = ACTIONINFO_NULL;					
			}
		}

		
		
		if (actionInfoAction!=ACTION_STAND)
		{
			SetAction( actionInfoAction );
		}
	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	else if (m_bNextAction 
			|| m_NextAction!=ACTION_STAND 
			&& (!bSlayer || bSlayer && m_NextAction != ACTION_SLAYER_MOTOR_STAND)
			&& m_NextAction!=m_MoveAction)
	{
		SetAction( m_NextAction );
	}	 

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS)
		|| HasEffectStatus(EFFECTSTATUS_EXPLOSION_WATER)
		|| (HasEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS) && !IsSlayer())
		|| HasEffectStatus(EFFECTSTATUS_GLACIER))
	{			
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		{
			AffectUsedActionInfo(m_nUsedActionInfo);
			m_nUsedActionInfo	= ACTIONINFO_NULL;
		}
	}
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	else
	{
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (m_ActionCount < m_ActionCountMax)
		{		
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			if (m_nUsedActionInfo!=ACTIONINFO_NULL)
			{
				ActionEffect();
			}
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			else
			{
				
				//switch (m_Action)
				if (m_Action==ACTION_STAND 
					|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND)
				{
					m_ActionCount++;
				}

				 
				
				//	default :
				else 
				{
					ActionEffect();
				}
			}
		}
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		if (m_ActionCount>=m_ActionCountMax)
		{
			//--------------------------------------------------------
			
			//--------------------------------------------------------
			if (m_bAlive)
			{
				//--------------------------------------------------------
				
				
				
				//--------------------------------------------------------
				if (m_MoveCount==m_MoveCountMax)
				{		
					//#ifdef CONNECT_SERVER
					//--------------------------------------------------------
					
					//--------------------------------------------------------
					AffectMoveBuffer();

					BOOL bSlayer = IsSlayer();

					if (m_bNextAction 
						|| m_NextAction!=ACTION_STAND 
						&& (!bSlayer || bSlayer && m_NextAction!=ACTION_SLAYER_MOTOR_STAND))
					{
						m_Action = ((m_MoveDevice==MOVE_DEVICE_RIDE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND);
						SetAction( m_NextAction );
					}	 
					else
					{
						SetAction( (m_MoveDevice==MOVE_DEVICE_RIDE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );				
					}
					//#else
					//	SetAction( m_MoveAction );				
					//#endif
				}
				//--------------------------------------------------------
				
				
				//--------------------------------------------------------
				else 
				{			
					m_Action = m_MoveAction;				
					
					ActionMove();

					if (m_MoveCount>=m_MoveCountMax)
					{
						
//						if (m_NextX != SECTORPOSITION_NULL)
						if (m_NextX != SECTORPOSITION_NULL
							&& m_NextDirection != DIRECTION_NULL)
						{		
//							DEBUG_ADD_FORMAT("[ Apply Buffering Move ] [ID=%d] Current(%d, %d) Dir(%d) --> Next(%d, %d) Dir(%d)", 
//															m_ID,
//															m_X, m_Y, m_Direction, 
//															m_NextX,m_NextY, m_NextDirection);
//							
//							MovePosition( m_NextX, m_NextY );
//							m_Direction = m_NextDirection;					
//							

//							SetAction( m_MoveAction );
//
//							m_NextX = SECTORPOSITION_NULL;
//							m_NextY = SECTORPOSITION_NULL;
//							m_NextDirection = DIRECTION_NULL;
							MovePosition( m_NextX, m_NextY );
							int dir = m_Direction;

							SetAction( m_MoveAction );
							m_Direction = m_NextDirection;
							m_DirectionMove = m_NextDirection;
							ActionMove();
							m_Direction = dir;

							m_NextX = SECTORPOSITION_NULL;
							m_NextY = SECTORPOSITION_NULL;
							m_NextDirection = DIRECTION_NULL;
						}
						else
						{
							//#ifdef CONNECT_SERVER

							//--------------------------------------------------------
							
							//--------------------------------------------------------
							AffectMoveBuffer();

							BOOL bSlayer = IsSlayer();

							if (m_bNextAction 
								|| m_NextAction!=ACTION_STAND 
								&& (!bSlayer || bSlayer && m_NextAction!=ACTION_SLAYER_MOTOR_STAND))
							{
								m_Action = ((m_MoveDevice==MOVE_DEVICE_RIDE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND);
								SetAction( m_NextAction );
							}	 
							else
							{
								SetAction( (m_MoveDevice==MOVE_DEVICE_RIDE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );				
							}
							//#else
							//	SetAction( m_MoveAction );				
							//#endif
						}
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MFakeCreature::SetFakeCreatureFastMoveAction(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY,
											 TYPE_ACTIONINFO skillID, 
											 TYPE_OBJECTID targetID)
{
	FastMovePosition(sX, sY);

	m_nBasicActionInfo = m_nSpecialActionInfo = skillID;
	m_TraceID = targetID;
}


//----------------------------------------------------------------------
// Set FakePosition
//----------------------------------------------------------------------
void				
MFakeCreature::SetFakePosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)	
{ 
	m_FakeX = sX; 
	m_FakeY = sY; 

	switch (m_FakeCreatureType)
	{
		//----------------------------------------------
		
		//----------------------------------------------
		case FAKE_CREATURE_TURNING_MOVE :
		{
			//int actionCountMax = 2 * (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
			int actionCountMax = 2 * GetCreatureActionCountMax( this, m_Action );

			if (actionCountMax==0)
			{
				actionCountMax = 1;
			}

			
			// gapX = 200 - 100 = 100
			// m_cX = 100 / c
			int gapX = m_FakeX - m_X;
			int gapY = m_FakeY - m_Y;
			float step = actionCountMax;
			m_FakeCX = gapX*TILE_X / step;
			m_FakeCY = gapY*TILE_Y / step;

			m_sX = 0;
			m_sY = 0;
			m_FakeSX = 0.0f;
			m_FakeSY = 0.0f;
		}
		break;
	}
}

//----------------------------------------------------------------------
// Update Fake
//----------------------------------------------------------------------
void
MFakeCreature::UpdateFake()
{
	switch (m_FakeCreatureType)
	{
		//----------------------------------------------
		
		//----------------------------------------------
		case FAKE_CREATURE_TURNING_MOVE :
		{
			m_Direction = ((m_Direction+1) & 0x07);

			if (m_X!=m_FakeX || m_Y!=m_FakeY)
			{			
				m_FakeSX += m_FakeCX;
				m_FakeSY += m_FakeCY;

				int sX = m_FakeSX;
				int sY = m_FakeSY;

				
				if (abs(sX) >= TILE_X || abs(sY) >= TILE_Y)
				{
					
					int mapX = m_X;
					int mapY = m_Y;
					int	pixelX = MTopView::MapToPixelX(mapX);
					int	pixelY = MTopView::MapToPixelY(mapY);

					pixelX += sX;
					pixelY += sY;

					m_X = MTopView::PixelToMapX(pixelX);
					m_Y = MTopView::PixelToMapY(pixelY);

					sX %= TILE_X;
					sY %= TILE_Y;

					m_FakeSX = sX;
					m_FakeSY = sY;
				}
			}			
			else
			{
				m_FakeSX = 0;
				m_FakeSY = 0;
				m_FakeCX = 0;
				m_FakeCY = 0;
			}
		}
		break;
	}
}

//----------------------------------------------------------------------
// IsFakeEnd
//----------------------------------------------------------------------
bool
MFakeCreature::IsFakeEnd()
{
	switch (m_FakeCreatureType)
	{
		case FAKE_CREATURE_FADE_ACTION :
			return (m_Action==ACTION_STAND
					|| m_ActionCount>=m_ActionCountMax);
		break;

		case FAKE_CREATURE_TO_PORTAL :
			return m_CutHeightCount >= m_CutHeightCountLast;
		break;

		case FAKE_CREATURE_TURNING_MOVE :
			return m_X==m_FakeX 
					&& m_Y==m_FakeY 
					&& m_ActionCount>=m_ActionCountMax;
		break;

		
		case FAKE_CREATURE_FAST_MOVE_ACTION :
			return !m_bFastMove
					&& m_nUsedActionInfo==ACTIONINFO_NULL
					&& m_nNextUsedActionInfo==ACTIONINFO_NULL
					&& (m_Action==ACTION_STAND
						|| m_ActionCount>=m_ActionCountMax);
		break;
		case FAKE_CREATURE_ROCKET:
			{
				MCreature *pCreature = g_pZone->GetCreature(m_TraceID);	
//			if(pCreature != NULL)
//			{
//				if(GetX() == pCreature->GetX() && GetY() == pCreature->GetY())
//				{
//					ExecuteActionInfoFromMainNode(RESULT_TEMP_SKILL_PLASMA_ROCKET_LAUNCHER,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	0,	
//							pCreature->GetX(), pCreature->GetY(), 0, 10, NULL, false);			
//					return true;
//				}
//			}
				if( timeGetTime() > m_nextMoveTime || NULL == pCreature)
				{
					if(pCreature != NULL)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_PLASMA_ROCKET_LAUNCHER_BLOW,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	0,	
								pCreature->GetX(), pCreature->GetY(), 0, 10, NULL, false);			
					return true;
				}
			}
			break;
		case FAKE_CREATURE_WILD_WOLF:
			{
				if(WILD_WOLF_MODE_REMOVE == m_WildWolf_Mode && m_PatrolCount > 30)
					return true;
			}
			break;
		case FAKE_CREATURE_DRAGON_TORNADO:
			{
				POINT	firstSector, endSector;
				firstSector.x = g_pPlayer->GetX() + g_SECTOR_SKIP_PLAYER_LEFT-1; 
				firstSector.y = g_pPlayer->GetY() + g_SECTOR_SKIP_PLAYER_UP-1;
				endSector.x	  = firstSector.x+g_SECTOR_WIDTH+2;
				endSector.y   = firstSector.y+g_SECTOR_HEIGHT+2;
				
				if(!IsInSector(firstSector.x, firstSector.y, endSector.x, endSector.y))
					return true;
			}
			break;
	}

	return false;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MFakeCreature::PacketSpecialActionToOther(TYPE_ACTIONINFO nActionInfo, TYPE_OBJECTID id, MActionResult* pActionResult)
{
	MCreature::PacketSpecialActionToOther(nActionInfo, id, pActionResult);

	switch (m_FakeCreatureType)
	{
		case FAKE_CREATURE_FAST_MOVE_ACTION :
		{		
			int dir = GetDirectionToPosition( m_TraceX, m_TraceY );
			SetDirection( dir );
			SetCurrentDirection( dir );
		}
		break;
	}
}

void
MFakeCreature::KeepTraceCreature()
{
	MCreature *pCreature = g_pZone->GetCreature(m_TraceID);

	if(pCreature != NULL)
	{
		UpdateTurret();

		int distance = max(abs(pCreature->GetX()-m_X), abs(pCreature->GetY()-m_Y));
		if(distance > 6)
		{
			m_FinalTurretDirect = 0xFF;
			RemoveNodes();
			m_bTraceFlag = true;
			SetFakeCreatureFastMoveAction(pCreature->GetX(), pCreature->GetY(), ACTIONINFO_NULL, pCreature->GetID());
		}
		else
		if(distance > 3)
		{
			m_FinalTurretDirect = 0xFF;
			m_bTraceFlag = true;
			RemoveNodes();
			SetNextDestination(pCreature->GetX(), pCreature->GetY());
			
			if(m_Action == ACTION_VAMPIRE_DRAIN)
			{
				if(m_ActionCount < m_ActionCountMax-13)
					m_ActionCount = m_ActionCountMax-13;
				
				SetNextActionToMove();
			}
			else
			{
				if( m_Action != ACTION_MOVE )
					SetAction(m_MoveAction);
			}
		}

		static BYTE lastAction = ACTION_STAND;
		if(lastAction != ACTION_STAND && m_Action == ACTION_STAND)
		{
			
			m_bTraceFlag = false;
		}
		lastAction = m_Action;

		if(m_bFastMove == false && timeGetTime() >= m_nextMoveTime)
		{
			if(m_PatrolCount <= 0)
			{
				MPetItem *pPetItem = GetPetItem();
				if(pPetItem != NULL)
				{
					switch(pPetItem->GetItemType())
					{
					case 1:
					case 2:
						m_nextMoveTime = timeGetTime()+(rand()%5+3)*1000;
						m_PatrolCount = rand()%5+3;
						SetAction(ACTION_VAMPIRE_DRAIN);
						break;

					case 3:
						m_PatrolCount = rand()%3+2;
						m_nextMoveTime = timeGetTime()+(rand()%3+2)*1000;
						ExecuteActionInfoFromMainNode(
							SKILL_CLIENT_TANK_SMOKE,										
							
							0, 0, 0,
							DIRECTION_DOWN, 
							
							GetID(),												
							0, 0, 0, 
							
							0,													
							
							NULL,
							
							false);	
						break;
					}
				}
			}
			else if(m_Action == ACTION_STAND && m_PatrolCount > 0 && GetPetItem() != NULL)
			{
				MPetItem *pPetItem = GetPetItem();

				if(GetCreatureType() == 702 || GetCreatureType() == 703 || GetCreatureType() == 704)
				{
					SetTurretFinalDirection( rand()/(RAND_MAX/32) );
					m_nextMoveTime = timeGetTime()+(rand()%3+2)*1000;
					m_PatrolCount--;
				}
				else
				{
					m_nextMoveTime = timeGetTime()+(rand()%5+3)*1000;
					m_PatrolCount--;
					int minDistance = 1;

					if(pPetItem != NULL)
					{
					int petLevel = GetPetItem()->GetNumber();

					if(petLevel >= 30)
						minDistance = 2;
					else if(petLevel >= 20)
						minDistance = 1;
					}

					POINT p = CaculatePetPosition(pCreature->GetX(), pCreature->GetY(), 2, minDistance, GetMoveType(), GetX(), GetY());
					SetNextDestination(p.x, p.y);
					SetAction( m_MoveAction );
				}
			}
		}


//		else if(distance <=1)
//			m_bTraceFlag = false;
//
//		if(m_bTraceFlag)
//		{		
//			BYTE dir = GetDirectionToPosition(pCreature->GetX(), pCreature->GetY());
//			TYPE_SECTORPOSITION x = m_X, y = m_Y;
//			GetPositionToDirection(x, y, dir);
//			PacketMove(x, y, dir);
//			SetNextDestination(pCreature->GetX(), pCreature->GetY());
//			SetAction( m_MoveAction );
//
//		}
	}
	else
	{
		SetStop();
	}
}

//----------------------------------------------------------------------
// Set Next Destination
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool	
MFakeCreature::SetNextDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{ 
	
	
	if (m_bTraceFlag==false && sX==m_X && sY==m_Y)
	{
		return false;
	}	

	
	
	if (sX==SECTORPOSITION_NULL || sY==SECTORPOSITION_NULL
		|| sX==m_BlockDestX && sY==m_BlockDestY)
	{
		return false;
	}

	if(m_bTraceFlag == false)
	{
		BOOL bCanStand = m_pZone->CanMove(m_MoveType, sX,sY);
		if(!bCanStand)
		{
			extern POINT g_DirectionValue[MAX_DIRECTION];
			
			POINT MovePoint;

			if( sY > GetY() )
				MovePoint.y = -1;
			else if( sY < GetY() )
				MovePoint.y = 1;
			else
				MovePoint.y = 0;

			if( sX > GetX() )
				MovePoint.x = -1;
			else if( sX < GetX() )
				MovePoint.x = 1;
			else
				MovePoint.x = 0;

			int limit = 20;	

			while( bCanStand == FALSE && !( GetX() == sX && GetY() == sY ) &&
				sX >= 0 && sY >= 0 
				&& sX < m_pZone->GetWidth() && sY < m_pZone->GetHeight()
				&& limit-- > 0
				)
			{
				if( GetX() != sX )sX += MovePoint.x;
				if( GetY() != sY )sY += MovePoint.y;
				bCanStand = m_pZone->CanMove( m_MoveType, sX,sY );
			};

			if( bCanStand == FALSE || GetX() == sX && GetY() == sY)
				return false;
		}
	}

	
	m_NextDestX = sX; 
	m_NextDestY = sY; 

//	SetDestination(m_NextDestX, m_NextDestY);

	return true;
}

//----------------------------------------------------------------------
// Get NextDestination()
//----------------------------------------------------------------------
void
MFakeCreature::GetNextDestination(POINT &dest)
{
	dest.x = m_NextDestX;
	dest.y = m_NextDestY;
}

//----------------------------------------------------------------------
// Get NextPosition()
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
bool
MFakeCreature::GetNextPosition(POINT &next)
{
	
	if (m_listDirection.empty())
	{
		//next.x = SECTORPOSITION_NULL;
		//next.y = SECTORPOSITION_NULL;
		return false;
	}

	
	

	
	next.x = m_X;
	next.y = m_Y;
	
	switch (m_listDirection.front())
	{
		case DIRECTION_LEFTDOWN		: next.x--;	next.y++;	break;
		case DIRECTION_RIGHTUP		: next.x++;	next.y--;	break;
		case DIRECTION_LEFTUP		: next.x--;	next.y--;	break;
		case DIRECTION_RIGHTDOWN	: next.x++;	next.y++;	break;
		case DIRECTION_LEFT			: next.x--;				break;
		case DIRECTION_DOWN			:			next.y++;	break;
		case DIRECTION_UP			:			next.y--;	break;
		case DIRECTION_RIGHT		: next.x++;				break;
	}

	return true;
}


//----------------------------------------------------------------------
// Set Destination(sX, sY)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
MFakeCreature::SetDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

	// message : find path
	//DEBUG_ADD_FORMAT("........Find Path........");				

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD_FORMAT("SetDest(%d, %d)", sX, sY);
	#endif

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (!m_bAlive)
	{
		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("dead");
		#endif
		return false;
	}

	//-------------------------------------------------------
	// if not empty, delete all elements
	//-------------------------------------------------------
	if (!m_listDirection.empty())
	{
		m_listDirection.clear();
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (sX<0 || sY<0 
		|| sX>=m_pZone->GetWidth() || sY>=m_pZone->GetHeight()
		)
	{
		
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;
		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("outOfZone or Sanctuary");
		#endif

		return false;

		
		 
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (m_bTraceFlag == true
		&& max(abs(m_TraceX-m_X), abs(m_TraceY-m_Y))<=1)
	{
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;

		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;
	
		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("already in dist");
		#endif

		return false;
	}

	/*
	// priority queue test code
	ofstream file("test.txt", ios::app);
	CString str;

	m_pqDNodes.push( new DNode(0,0,0,1) );
	m_pqDNodes.push( new DNode(0,0,0,4) );
	m_pqDNodes.push( new DNode(0,0,0,3) );
	m_pqDNodes.push( new DNode(0,0,0,8) );
	m_pqDNodes.push( new DNode(0,0,0,2) );
	m_pqDNodes.push( new DNode(0,0,0,4) );
	m_pqDNodes.push( new DNode(0,0,0,7) );

	while (!m_pqDNodes.empty())
	{
		DNode*	pDNode = m_pqDNodes.top();
		m_pqDNodes.pop();

		str.Format("Pop : (%d,%d) , [%d], dist=%d", pDNode->x, pDNode->y, pDNode->direction, pDNode->distance);
		
		file << str << endl;
	}

	return;
	*/

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	// Best First Search
	//--------------------------------------------------------------
	BOOL	bCanStand = m_pZone->CanMove(m_MoveType, sX,sY);

#ifdef __EXPO_CLIENT__
	bCanStand |= (BOOL)g_UserInformation.Invisible;
#endif

	//--------------------------------------------------------------
	
	
	//--------------------------------------------------------------
	if (bCanStand || m_bTraceFlag == true) 
	{		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		m_DestX = sX;
		m_DestY = sY;

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		RemoveNodes();	

		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		POINT firstSector = g_pTopView->GetFirstSector();
		//int x0 = m_DestX - g_pClientConfig->MAX_FINDPATH_WIDTH, 
		//	x1 = m_DestX + g_pClientConfig->MAX_FINDPATH_WIDTH+1, 
		//	y0 = m_DestY - g_pClientConfig->MAX_FINDPATH_HEIGHT,
		//	y1 = m_DestY + g_pClientConfig->MAX_FINDPATH_HEIGHT+1;
		int x0 = firstSector.x - 1, 
			x1 = firstSector.x + g_SECTOR_WIDTH + 5, 
			y0 = firstSector.y - 1,
			y1 = firstSector.y + g_SECTOR_HEIGHT + 5;

		if (x0 < 0)						
			x0 = 0;
		if (x1 > g_pZone->GetWidth())	
			x1 = g_pZone->GetWidth();
		
		if (y0 < 0)				
			y0 = 0;
		if (y1 > g_pZone->GetHeight())	
			y1 = g_pZone->GetHeight();
		

		for (int i=y0; i<y1; i++)
		{			
			for (int j=x0; j<x1; j++)
			{
				//m_ppVisited[i][j] = false;
				g_pZone->UnSetVisitedFlag(j, i);
			}
//			memset( m_ppVisited[i]+x0, false, size );
		}
			

		//-------------------------------------------------------
		//
		
		//
		//-------------------------------------------------------

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		static BYTE FirstDirection = GetDirectionToPosition(m_DestX, m_DestY);
		static TYPE_SECTORPOSITION previousDestX = SECTORPOSITION_NULL;
		static TYPE_SECTORPOSITION previousDestY = SECTORPOSITION_NULL;

		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		if (previousDestX!=m_DestX || previousDestY!=m_DestY)
		{
			FirstDirection = GetDirectionToPosition(m_DestX, m_DestY);
			previousDestX = m_DestX;
			previousDestY = m_DestY;
		}
		//-------------------------------------------------------
		
		
		//-------------------------------------------------------
		else
		{
			FirstDirection = m_Direction;
		}
		
		DNode*	currentDNode = new DNode(
									m_X,
									m_Y,
									FirstDirection,
									CalculateDistance(m_X, m_Y),
									0
									);
	
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		m_listDNodes.push_back( currentDNode );

		currentDNode->SetParent(NULL);
		m_pqDNodes.push(currentDNode);	
		
		POINT	next;
		int		dist;
		bool	bFound = false;		

		while (!bFound && !m_pqDNodes.empty())
		{
			//-------------------------------------------------------
			
			
			//-------------------------------------------------------
			DNode* pDNode = m_pqDNodes.top();
			m_pqDNodes.pop();

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (pDNode->distance==0) 
			{
				m_pqDNodes.push(pDNode);
				bFound = true;
				break;
			}
			
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (m_bTraceFlag == true && 
				max(abs(pDNode->x-m_DestX), abs(pDNode->y-m_DestY))<=1)
			{
				m_pqDNodes.push(pDNode);
				bFound = true;
				break;
			}			

			int Direction = pDNode->direction;

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			for (int i=0; i<8; Direction++, i++)
			{		
				if (Direction==8) Direction=0;

				next.x = pDNode->x;
				next.y = pDNode->y;

				switch (Direction)
				{
					case DIRECTION_LEFTDOWN		: next.x--;	next.y++;	break;
					case DIRECTION_RIGHTUP		: next.x++;	next.y--;	break;
					case DIRECTION_LEFTUP		: next.x--;	next.y--;	break;
					case DIRECTION_RIGHTDOWN	: next.x++;	next.y++;	break;
					case DIRECTION_LEFT			: next.x--;				break;
					case DIRECTION_DOWN			: next.y++;				break;
					case DIRECTION_UP			: next.y--;				break;
					case DIRECTION_RIGHT		: next.x++;				break;
				}

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (next.x<0 || next.y<0 
					|| next.x>=m_pZone->GetWidth() || next.y>=m_pZone->GetHeight()) continue;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (g_pZone->IsVisitedFlag(next.x, next.y)) continue;

				
				if (next.x<x0 || next.y<y0 
					|| next.x>=x1 || next.y>=y1) continue;

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (m_pZone->CanMove(m_MoveType, next.x, next.y))
				{				
					g_pZone->SetVisitedFlag(next.x, next.y);

					//-------------------------------------------------------
					
					//-------------------------------------------------------
					
					dist = CalculateDistance(next.x, next.y);

					
					DNode*	nextDNode = new DNode(
											next.x, 
											next.y, 
											Direction,
											dist,
											pDNode->step + 1);
					
				
					//-------------------------------------------------------
					
					//-------------------------------------------------------
					m_listDNodes.push_back( nextDNode );
					
					//CString str;
					//str.Format("NewNode : (%d,%d)-->(%d,%d) , [%d], dist=%d", next.x, next.y, m_Dest.x, m_Dest.y, i, dist);

					nextDNode->SetParent(pDNode);
					m_pqDNodes.push(nextDNode);					
				}

			
			}
		}

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (bFound)
		{				
			//-------------------------------------------------------
			
			//-------------------------------------------------------

			
			DNode* currentNode = m_pqDNodes.top();
			m_pqDNodes.pop();
			
			while (currentNode->pParent!=NULL)
			{
				m_listDirection.insert( m_listDirection.begin(), currentNode->direction);
				currentNode = currentNode->pParent;			
			}

			RemoveNodes();

			m_BlockDestX = SECTORPOSITION_NULL;
			m_BlockDestY = SECTORPOSITION_NULL;
		}
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		else
		{
			
			if (m_bTraceFlag == true)				
			{
				m_MoveCount = m_MoveCountMax;

				m_bTraceFlag = false;

				
				m_nUsedActionInfo = ACTIONINFO_NULL;
				m_nNextUsedActionInfo = ACTIONINFO_NULL;
			}

			m_BlockDestX = m_DestX;
			m_BlockDestY = m_DestY;

			m_DestX = SECTORPOSITION_NULL;
			m_DestY = SECTORPOSITION_NULL;			
		}
	}
	
	else 
	{
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;

		
		//m_BlockDestX = m_DestX;
		//m_BlockDestY = m_DestY;

		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;		

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("no way!");
		#endif

		return false;
	}

	 

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("find way");
	#endif

	return true;
}

//----------------------------------------------------------------------
// Get Destination()
//----------------------------------------------------------------------
void
MFakeCreature::GetDestination(POINT &dest)
{
	dest.x = m_DestX;
	dest.y = m_DestY;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MFakeCreature::RemoveNodes()
{
	while (!m_pqDNodes.empty())
	{
		m_pqDNodes.pop();
	}


	
	DNODE_LIST::iterator iNode = m_listDNodes.begin();

	while (iNode != m_listDNodes.end())
	{
		DNode* pNode = *iNode;
		delete pNode;
		iNode++;		
	}

	m_listDNodes.clear();
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
int
MFakeCreature::CalculateDistance(int x, int y)
{

	int xd = abs(x-m_DestX),
		yd = abs(y-m_DestY);	 

	int plus = !(m_DestX==x) + !(m_DestY==y);
		
	if (xd>yd)
	{
		return xd*xd + plus;
	}
	else return yd*yd + plus;

	return 0;
}

void
MFakeCreature::SetStop()
{
	
	m_listDirection.clear(); 

	
	m_bTraceFlag = false;
	//m_fNextTrace	= FLAG_TRACE_NULL;

	
	m_sX=0; 
	m_sY=0;

	
	m_NextX = SECTORPOSITION_NULL;
	m_NextY = SECTORPOSITION_NULL;

	
	m_DestX			= SECTORPOSITION_NULL; 
	m_DestY			= SECTORPOSITION_NULL; 
	m_NextDestX		= SECTORPOSITION_NULL; 
	m_NextDestY		= SECTORPOSITION_NULL; 

	m_bFastMove = false;
	m_bKnockBack = 0;	

	//------------------------------------------------
	
	//------------------------------------------------
	AffectMoveBufferAll();
	//ReleaseMoveBuffer();

	
	ActionMoveNextPosition();

	
	m_sX=0; 
	m_sY=0;

	int action = ACTION_STAND;
	m_Action		= action;
	m_ActionCount	= m_ActionCountMax; 
	m_MoveCount		= m_MoveCountMax;

	
	m_bNextAction = false;
	m_NextX = SECTORPOSITION_NULL;
	m_NextY = SECTORPOSITION_NULL;

	
	//m_nNextUsedActionInfo = ACTIONINFO_NULL;

}

//----------------------------------------------------------------------
// Move
//----------------------------------------------------------------------
//

//


//



//



//----------------------------------------------------------------------
void	
MFakeCreature::ActionMovePet()
{
	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("ActionMove");
	#endif

	int moveTimesTemp = 0;
	if (m_MoveDevice==MOVE_DEVICE_WALK)
	{
		moveTimesTemp = (*g_pCreatureTable)[m_CreatureType].MoveTimes;
	}
	else if (m_MoveDevice==MOVE_DEVICE_RIDE)
	{
		moveTimesTemp = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor;
	}
	else if (m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
	{
		moveTimesTemp = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor;
	}

	//----------------------------------------------------
	//
	
	//
	//----------------------------------------------------
	
	
	//----------------------------------------------------
	if (m_MoveCount%moveTimesTemp==0)//m_sX==0 && m_sY==0)
	{
		TYPE_SECTORPOSITION	x, y;

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		if (m_bFastMove)
		{
			x = m_NextX;
			y = m_NextY;

			m_NextX = SECTORPOSITION_NULL;					

			//m_CurrentDirection = 
			m_Direction = GetDirectionToPosition( x, y );
		}
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		else
		{
			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_DestX==SECTORPOSITION_NULL && m_NextDestX==SECTORPOSITION_NULL) 
			{
				DEBUG_ADD("### Don't need to move any more - Move Stop! ###");
				
				m_MoveCount = m_MoveCountMax;
				return;
			}

				//sprintf(g_pDebugMessage->GetCurrent(), "ListSize = %d", m_listDirection.size());
			
			//----------------------------------------------------
			//
			//
			
			//
			//
			//----------------------------------------------------		
			//----------------------------------------------------	
			
			
			
			//----------------------------------------------------	
			if (m_bTraceFlag == true) 
			{
				
				MCreature*	pCreature = m_pZone->GetCreature( m_TraceID );				

				//GET_DYING_CREATURE( pCreature, m_TraceID );

				//-------------------------------------------------------
				
				
				//-------------------------------------------------------								
				if (pCreature==NULL )
				{
					
					m_bTraceFlag = false;
						
					
					SetStop();					
					return;			
				}
			
				//-------------------------------------------------------			
				
				//-------------------------------------------------------			
				if (pCreature->GetX()!=m_TraceX ||
					pCreature->GetY()!=m_TraceY)
				{										
					m_NextDestX = pCreature->GetX();
					m_NextDestY = pCreature->GetY();
					
				}

				m_TraceX	= pCreature->GetX();
				m_TraceY	= pCreature->GetY();
				m_TraceZ	= pCreature->GetZ();
			}
			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_NextDestX!=SECTORPOSITION_NULL)
			{
				
				//SetDestination(m_NextDestX, m_NextDestY);
				m_DestX		= m_NextDestX;	// TEST
				m_DestY		= m_NextDestY;	// TEST
				m_NextDestX = SECTORPOSITION_NULL;
				m_NextDestY = SECTORPOSITION_NULL;			
			}

			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_DestX!=SECTORPOSITION_NULL)
				SetDestination(m_DestX, m_DestY);

			//----------------------------------------------------
			
			//----------------------------------------------------
			//SetAction( m_MoveAction );

			
			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_listDirection.empty())
			{
				//--------------------------------------------
				
				//--------------------------------------------
				if (m_DestX==m_X && m_DestY==m_Y)
				{
					m_DestX = SECTORPOSITION_NULL;
					m_DestY = SECTORPOSITION_NULL;

					#ifdef OUTPUT_DEBUG_PLAYER_ACTION
						DEBUG_ADD("dest reached");
					#endif

					return;
				}

				#ifdef OUTPUT_DEBUG_PLAYER_ACTION
					DEBUG_ADD("no way");
				#endif

				return;
			}		

			
			//----------------------------------------------------	
			//
			
			//
			//----------------------------------------------------			
			//----------------------------------------------------
			
			//----------------------------------------------------
			m_Direction = m_listDirection.front();
			m_listDirection.pop_front();	
				
			//----------------------------------------------------
			
			//----------------------------------------------------
			if (m_NextDestX!=SECTORPOSITION_NULL)
			{
				//m_NextAction = m_MoveAction;
				SetNextAction( m_MoveAction );
			}

			//----------------------------------------------------
			
			
			//----------------------------------------------------

			//----------------------------------------------------
			
			//----------------------------------------------------
			x = m_X;
			y = m_Y;

			switch (m_Direction)
			{
				case DIRECTION_LEFTDOWN		: x--;	y++;	break;
				case DIRECTION_RIGHTUP		: x++;	y--;	break;
				case DIRECTION_LEFTUP		: x--;	y--;	break;
				case DIRECTION_RIGHTDOWN	: x++;	y++;	break;
				case DIRECTION_LEFT			: x--;			break;
				case DIRECTION_DOWN			:		y++;	break;
				case DIRECTION_UP			:		y--;	break;
				case DIRECTION_RIGHT		: x++;			break;
			}
		}

		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		if (m_pZone->CanMove(m_MoveType, x,y)
			|| m_bFastMove		
			
			)
		{		
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			if (m_bFastMove)
			{
				// nothing
			}
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			else
			{
			}

			//-----------------------
			
			//-----------------------
			//m_pZone->MoveLightSight(m_X, m_Y, m_LightSight, m_Direction);
			//m_pZone->UnSetLightSight(m_X, m_Y, m_LightSight);
			//m_pZone->SetLightSight(x, y, m_LightSight);

			
			TYPE_SECTORPOSITION ox = m_X;
			TYPE_SECTORPOSITION oy = m_Y;

			
			MovePosition( x, y );
			
			//CheckInDarkness();

			//--------------------------------------------
			
			//--------------------------------------------
			//
			
			//
			
			
			// 
			//
			
			//
			
			
			//
			//--------------------------------------------
			
			
			//--------------------------------------------		
			//--------------------------------------------
			
			//--------------------------------------------

			if (m_bFastMove)
			{
				
				
				
				m_sX = (ox - m_X) * TILE_X;
				m_sY = (oy - m_Y) * TILE_Y;
				
				//------------------------------------------------
				
				//------------------------------------------------
				
				int moveTimes_div_2 = (*g_pCreatureTable)[m_CreatureType].MoveTimes >> 1;
				
				m_cX = -m_sX / moveTimes_div_2;
				m_cY = -m_sY / moveTimes_div_2;

				m_MoveCount += 2;

				if (m_MoveCount > m_MoveCountMax)
				{
					m_MoveCount = m_MoveCountMax;
				}
			}
			//--------------------------------------------	
			
			//--------------------------------------------
			else
			{	
				m_DirectionMoved = m_Direction;

				m_sX = m_sXTable[m_DirectionMoved];
				m_sY = m_sYTable[m_DirectionMoved];

				//????????????????????????????????????????????????????????
				//???                                                  ???
				
				//???                                                  ???
				//????????????????????????????????????????????????????????
				int moveTimes_1;
				
				#ifdef OUTPUT_DEBUG
					if (m_CreatureType >= g_pCreatureTable->GetSize())
					{
						DEBUG_ADD_FORMAT("[Error] CreatureType is exceed MAX. id=%d, type=%d", m_ID, m_CreatureType);
						
						SetStop();

						return;
					}
				#endif

				if (m_MoveDevice==MOVE_DEVICE_WALK)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimes-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_RIDE)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else
				{					
					DEBUG_ADD_FORMAT("[Error] What is MoveType of this Player??? id=%d", m_ID);							
					
					SetStop();

					return;
				}

				m_MoveTableCount = 0;
				m_cX = m_cXTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount%moveTimes_1];
				m_cY = m_cYTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount%moveTimes_1];
			}

			m_sX += m_cX;
			m_sY += m_cY;

			if( m_MoveCount == 0 )
				m_NextMoveCount = 1;//(*g_pCreatureTable)[m_CreatureType].MoveRatio;
			else
				m_NextMoveCount += 1;//(*g_pCreatureTable)[m_CreatureType].MoveRatio;

			m_MoveCount++;

			

			//------------------------------------------------
			
			//------------------------------------------------
			TYPE_SOUNDID soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( m_MoveAction );

			if (soundID!=SOUNDID_NULL)
			{
				PlaySound( soundID,
							false,
							m_X,
							m_Y);
			}
		}	
		//--------------------------------------------
		
		//--------------------------------------------
		
		else
		{
			m_NextDestX = m_DestX;
			m_NextDestY = m_DestY;
			m_listDirection.empty();


			DEBUG_ADD("   Block.");					
		}

		
		if (!m_listDirection.empty())
		{
			//m_NextAction = m_MoveAction;
			SetNextAction( m_MoveAction );		
		}
	}
	//----------------------------------------------------
	//
	
	//
	//----------------------------------------------------
	else
	{
		
		/*
		switch (m_MoveDevice)
		{
			case MOVE_DEVICE_NULL :			
				m_sX += m_cXTableDeviceNull[m_Direction][m_ActionCount];
				m_sY += m_cYTableDeviceNull[m_Direction][m_ActionCount];
			break;

			case MOVE_DEVICE_MOTOR1 :						
				m_sX += m_cXTableDeviceMotor1[m_Direction][m_ActionCount];
				m_sY += m_cYTableDeviceMotor1[m_Direction][m_ActionCount];
			break;
		}
		*/

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("going");
		#endif

		
		if (m_MoveCount>=m_NextMoveCount)
		{			
			//--------------------------------------------
			
			//--------------------------------------------
			if (m_bFastMove)
			{
				
				if (m_sX!=0)	m_sX += m_cX;
				if (m_sY!=0)	m_sY += m_cY;		

				m_MoveCount += 2;

				if (m_MoveCount > m_MoveCountMax)
				{
					m_MoveCount = m_MoveCountMax;
				}

				
				m_NextMoveCount += 1;//(*g_pCreatureTable)[m_CreatureType].MoveRatio;

			}
			//--------------------------------------------
			
			//--------------------------------------------
			else
			{
				int moveTimes_1;
				
				if (m_MoveDevice==MOVE_DEVICE_WALK)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimes-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_RIDE)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this)))
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] What is MoveType of this Creature??? id=%d", m_ID);					
					SetStop();					
					return;
				}

				m_MoveTableCount++;
				m_cX = m_cXTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount%moveTimes_1];
				m_cY = m_cYTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount%moveTimes_1];

				
				if (m_sX!=0)	m_sX += m_cX;
				if (m_sY!=0)	m_sY += m_cY;		

				
				m_NextMoveCount += 1;//(*g_pCreatureTable)[m_CreatureType].MoveRatio;

				m_MoveCount++;
			}
		}	
		else
		{
			m_MoveCount++;
		}
		
		//m_ActionCount++;
		
		//if (++m_ActionCount == (*m_pFrames)[m_Action][m_Direction].GetCount())
		//	m_ActionCount = 0;

		//if (m_ActionCount == (m_ActionCountMax>>1))
		//if (m_MoveCount == (m_MoveCountMax>>1))
		//{				
		//	SetLightSight( m_LightSight );
		//}
	}

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("moveok");
	#endif
}


void MFakeCreature::ActionMove()
{
	if(GetOwnerID() != OBJECTID_NULL)
		ActionMovePet();
	else
		MCreature::ActionMove();
}

void	
MFakeCreature::SetZone(MZone* pZone)
{
	// base class functions
	MCreature::SetZone(pZone);
}

void
MFakeCreature::SetOwnerID(TYPE_OBJECTID id)
{
	m_OwnerID = id;
}

POINT
MFakeCreature::CaculatePetPosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, int distance, int distanceMin, MOVE_TYPE moveType, TYPE_SECTORPOSITION petX, TYPE_SECTORPOSITION petY)
{
	POINT p = {sX, sY};

	if(g_pZone == NULL)
	{
		return p;
	}

	const int x0 = sX-distance;
	const int x1 = sX+distance;
	const int y0 = sY-distance;
	const int y1 = sY+distance;

	std::vector<POINT> vMovePoint;

	for (int i=y0; i<y1; i++)
	{			
		for (int j=x0; j<x1; j++)
		{
			if(g_pZone->CanMove(moveType, j, i) && j != sX && i != sY && max(abs(i-petY), abs(j-petX)) >= distanceMin)
			{
				POINT tempPoint = {j, i};
				vMovePoint.push_back(tempPoint);
			}
		}
	}

	if(!vMovePoint.empty())
	{
		p = vMovePoint[rand()%vMovePoint.size()];
	}

	return p;
}

void
MFakeCreature::SyncTurretDirection()
{
	m_TurretDirect = ((GetDirection()+6)%8)*4;
	m_FinalTurretDirect = 0xFF;
}

const BYTE
MFakeCreature::GetTurretDirection() const
{
	return m_TurretDirect;
}

void
MFakeCreature::SetTurretFinalDirection(BYTE d)
{
	m_FinalTurretDirect = d;
}

void
MFakeCreature::UpdateTurret()
{
	if( (GetCreatureType() == 702 || GetCreatureType() == 703 || GetCreatureType() == 704) )
	{
		if( GetTurretDelay() > 0 )
			m_TurretDelay--;
		
		bool bSetFinalDirect = false;

		TYPE_OBJECTID ownerID = GetOwnerID();

		BYTE finalDirect = 0;
		
		MCreature *pOwnerCreature = g_pZone->GetCreature( ownerID );

		if( pOwnerCreature != NULL )
		{
			TYPE_OBJECTID targetID = pOwnerCreature->GetStatus( MODIFY_LAST_TARGET );

			MCreature *pTargetCreature = g_pZone->GetCreature( targetID );

			if( pTargetCreature != NULL )
			{
				BYTE direct = GetDirectionToPosition( pTargetCreature->GetX(), pTargetCreature->GetY() );

				bSetFinalDirect = true;
				finalDirect = ((direct+6)%8)*4;
			}
		}
		
		if( bSetFinalDirect == false )
		{
			finalDirect = ((GetDirection()+6)%8)*4;
			if( m_FinalTurretDirect != 0xFF )
				finalDirect = m_FinalTurretDirect;
		}
			
		if( finalDirect != m_TurretDirect )
		{
			if( ((finalDirect - m_TurretDirect +32)%32) < 16 )
			{
				m_TurretDirect = (m_TurretDirect + 1)%32;
			}
			else
			{
				m_TurretDirect = (m_TurretDirect - 1 +32)%32;
			}
		}
	}
}
//2004, 03, 22 sobeit add start
void 
MFakeCreature::KeepWanderCreature()
{
	
	if(GetCreatureType() != CREATURETYPE_GHOST)
		return;
	POINT	firstSector, endSector,NextMove;
	firstSector.x = g_pPlayer->GetX() + g_SECTOR_SKIP_PLAYER_LEFT-1;
	firstSector.y = g_pPlayer->GetY() + g_SECTOR_SKIP_PLAYER_UP-1;
	endSector.x	  = firstSector.x+g_SECTOR_WIDTH+2;
	endSector.y   = firstSector.y+g_SECTOR_HEIGHT+2;
	
	if(IsInSector(firstSector.x, firstSector.y, endSector.x, endSector.y))
	{
		NextMove.x = GetX(); NextMove.y = GetY();
		switch (m_Direction)
		{
			case DIRECTION_LEFTDOWN		: NextMove.x--;	NextMove.y++;	break;
			case DIRECTION_RIGHTUP		: NextMove.x++;	NextMove.y--;	break;
			case DIRECTION_LEFTUP		: NextMove.x--;	NextMove.y--;	break;
			case DIRECTION_RIGHTDOWN	: NextMove.x++;	NextMove.y++;	break;
			case DIRECTION_LEFT			: NextMove.x--;				break;
			case DIRECTION_DOWN			:			NextMove.y++;	break;
			case DIRECTION_UP			:			NextMove.y--;	break;
			case DIRECTION_RIGHT		: NextMove.x++;				break;
		}

		if(	!m_pZone->CanMove(m_MoveType, NextMove.x,NextMove.y))
			m_Direction = (m_Direction + 3 + rand()%3)%8;
		
		if( m_Action != ACTION_MOVE )
		{
			if(0xff == m_DirectionMove)
				m_Direction = rand()%8;
			SetAction(ACTION_MOVE);
		}
		m_DirectionMove = m_Direction;
	}
	else
	{
		if(ACTION_MOVE == g_pPlayer->GetAction())
		{
			//SetStop();
			SetAction(ACTION_STAND);
			m_DirectionMove = 0xff;
		}
		else
		{
			if(abs(GetX()-g_pPlayer->GetX())>10 ||abs(GetY()-g_pPlayer->GetY())>15 )
			{
				//SetStop();
				SetAction(ACTION_STAND);
				return;
			}
			if(IsInSector(firstSector.x-1, firstSector.y, firstSector.x-1, endSector.y))
			{
				m_Direction = 3 + rand()%3;
				m_DirectionMove = m_Direction;
				SetAction(ACTION_MOVE);
			}
			else if(IsInSector(endSector.x+1, firstSector.y, endSector.x+1, endSector.y))
			{
				m_Direction = (7 + rand()%3)%8;
				m_DirectionMove = m_Direction;
				SetAction(ACTION_MOVE);
			}
			else if(IsInSector(firstSector.x, firstSector.y-1, endSector.x, firstSector.y-1))
			{
				m_Direction = 1 + rand()%3;
				m_DirectionMove = m_Direction;
				SetAction(ACTION_MOVE);
			}
			else if(IsInSector(firstSector.x, endSector.y+1, endSector.x, endSector.y+1))
			{
				m_Direction = 5 + rand()%3;
				m_DirectionMove = m_Direction;
				SetAction(ACTION_MOVE);
			}
			else 
				//SetStop();
				SetAction(ACTION_STAND);
			
		}
	}


}
bool
MFakeCreature::IsInSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_SECTORPOSITION eX, TYPE_SECTORPOSITION eY)
{
	if (GetX() >= sX && GetX() <= eX
		&& GetY() >= sY && GetY() <= eY)
	{
		return true;
	}
	return false;
}
//2004, 03, 22 sobeit add end

// 2004, 11, 8, sobeit add start
void 
MFakeCreature::KeepRocketLuncher()
{
	if(GetCreatureType() != CREATURETYPE_ROCKET_LUNCHER)
		return;

	if(IsGhost(0xff))
		return;
	MCreature *pCreature = g_pZone->GetCreature(m_TraceID);	
	if(pCreature != NULL)
	{
		int distance = max(abs(pCreature->GetX()-m_X), abs(pCreature->GetY()-m_Y));

		if(distance > 1)
		{
			m_DirectionMove = m_Direction = m_CurrentDirection = GetDirectionToPosition(pCreature->GetX(), pCreature->GetY());
			m_DestX = pCreature->GetX();
			m_DestY = pCreature->GetY();
			m_PatrolCount = m_CurrentDirection;
		}

		else
		{
			if(m_DestX == GetX() && m_DestY == GetY())
			{
				m_PatrolCount ++;
				m_DestX = pCreature->GetX(); m_DestY = pCreature->GetY();
				switch (m_PatrolCount % 8)
				{
					case DIRECTION_LEFTDOWN		: m_DestX--;	m_DestY++;	break;
					case DIRECTION_RIGHTUP		: m_DestX++;	m_DestY--;	break;
					case DIRECTION_LEFTUP		: m_DestX--;	m_DestY--;	break;
					case DIRECTION_RIGHTDOWN	: m_DestX++;	m_DestY++;	break;
					case DIRECTION_LEFT			: m_DestX--;				break;
					case DIRECTION_DOWN			:			m_DestY++;	break;
					case DIRECTION_UP			:			m_DestY--;	break;
					case DIRECTION_RIGHT		: m_DestX++;				break;
				}
			}
			m_DirectionMove = m_Direction = m_CurrentDirection = GetDirectionToPosition(m_DestX, m_DestY);
		}
	}

	if( m_Action != ACTION_MOVE )
		SetAction(m_MoveAction);

			

}
// 2004, 11, 8, sobeit add end

void MFakeCreature::SetFakeMoveOnce(BYTE Dir)
{
	m_DirectionMove = m_Direction = m_CurrentDirection = Dir%8;
	SetAction(m_MoveAction);
}

void
MFakeCreature::KeepWildWolfAttack()
{
	if(g_pZone == NULL)
		return;
	
	if(m_PatrolCount++ < 4 ) return;
	if(m_IsEatCorps)
	{
		if((GetX() == m_DestX && GetY() ==m_DestY) || m_WildWolf_Mode == 0 ) 
		{
			switch(m_WildWolf_Mode)
			{
				case WILD_WOLF_MODE_ATTACK_1st:
				case WILD_WOLF_MODE_ATTACK_2nd:
				case WILD_WOLF_MODE_ATTACK_3rd:
				{
					int TempDir = MTopView::GetDirectionToPosition(GetX(), GetY(), m_DestX, m_DestY);
					POINT TargetPoint = GetNextTileByDirection(m_DestX, m_DestY, TempDir);
					m_bTraceFlag = true;
					SetCurrentDirection(TempDir);
					SetFakeCreatureFastMoveAction(TargetPoint.x, TargetPoint.y, ACTIONINFO_NULL, NULL);	
					//SetNextDestination(TargetPoint.x, TargetPoint.y);
					m_DestX = TargetPoint.x;
					m_DestY = TargetPoint.y;
					m_WildWolf_Mode ++;
					m_DirectionMove = TempDir;
					if( m_Action != ACTION_MOVE )
						SetAction(ACTION_MOVE);
					m_PatrolCount = 0;
				}
				break;
				case WILD_WOLF_MODE_MOVE_1st:
				case WILD_WOLF_MODE_MOVE_2st:
				{
					int TempDir = (GetDirection()+4)%8;
					SetDirection(TempDir);
					SetAction(ACTION_VAMPIRE_DRAIN);
					m_PatrolCount = 0;
					m_WildWolf_Mode = WILD_WOLF_MODE_REMOVE;
				}
				break;
			}
			
		}
	}
	else
	{
		MCreature *pCreature = g_pZone->GetCreature(m_TraceID);
		if(NULL != pCreature)
		{
			if((GetX() == m_DestX && GetY() ==m_DestY) || m_WildWolf_Mode == 0 )
			{
				switch(m_WildWolf_Mode)
				{
				case WILD_WOLF_MODE_ATTACK_1st:
				case WILD_WOLF_MODE_ATTACK_2nd:
				case WILD_WOLF_MODE_ATTACK_3rd:
					{
						int TempDir = MTopView::GetDirectionToPosition(GetX(), GetY(), pCreature->GetX(), pCreature->GetY());
						POINT TargetPoint = GetNextTileByDirection(pCreature->GetX(), pCreature->GetY(), TempDir);
						m_bTraceFlag = true;
						SetCurrentDirection(TempDir);
						SetFakeCreatureFastMoveAction(TargetPoint.x, TargetPoint.y, ACTIONINFO_NULL, pCreature->GetID());	
						//SetNextDestination(TargetPoint.x, TargetPoint.y);
						m_WildWolf_Mode ++;
						m_DestX = TargetPoint.x;
						m_DestY = TargetPoint.y;
						m_DirectionMove = TempDir;
						if( m_Action != ACTION_MOVE )
							SetAction(ACTION_MOVE);
						m_PatrolCount = 0;
					}
					break;
				case WILD_WOLF_MODE_MOVE_1st:
				case WILD_WOLF_MODE_MOVE_2st:
					{
						int TempDir = MTopView::GetDirectionToPosition(GetX(), GetY(), pCreature->GetX(), pCreature->GetY());
						TempDir = (TempDir +7)%8;
						POINT TargetPoint = GetNextTileByDirection(GetX(), GetY(), TempDir );
						m_bTraceFlag = true;
						SetCurrentDirection(TempDir);
						SetFakeCreatureFastMoveAction(TargetPoint.x, TargetPoint.y, ACTIONINFO_NULL, pCreature->GetID());	
						//SetNextDestination(TargetPoint.x, TargetPoint.y);
						m_WildWolf_Mode ++;
						m_DestX = TargetPoint.x;
						m_DestY = TargetPoint.y;
						m_DirectionMove = TempDir;
						if( m_Action != ACTION_MOVE )
							SetAction(ACTION_MOVE);
						m_PatrolCount = 0;
					}
					break;
				}
				
			}
		}
		else
		{
			m_WildWolf_Mode = WILD_WOLF_MODE_REMOVE;
		}
		
	}
}

