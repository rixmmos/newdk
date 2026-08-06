//----------------------------------------------------------------------
// MEffectTarget.h
//----------------------------------------------------------------------
//

//

//
//  = EffectTargetNode

//    - CreatureID
//
//----------------------------------------------------------------------

#ifndef	__MEFFECTTARGET_H__
#define	__MEFFECTTARGET_H__

#pragma warning(disable:4786)


#include "MTypeDef.h"
#include "MActionResult.h"
#include "MString.h"

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MEffectTarget {
	public :
		enum EFFECT_TARGET_TYPE
		{
			EFFECT_TARGET_NORMAL,
			EFFECT_TARGET_PORTAL
		};

	public :
		//--------------------------------
		// assign
		//--------------------------------		
		MEffectTarget(const MEffectTarget& target);
		MEffectTarget(BYTE max);
		virtual ~MEffectTarget();

		virtual EFFECT_TARGET_TYPE	GetEffectTargetType() const	{ return EFFECT_TARGET_NORMAL; }
	
		//-------------------------------------------------------
		// Instance ID
		//-------------------------------------------------------
		void		NewEffectID()			{ m_EffectID = s_EffectID++; }
		BYTE		GetEffectID() const		{ return m_EffectID; }

		//-------------------------------------------------------
		// Set
		//-------------------------------------------------------
		void		Set(int x, int y, int z, TYPE_OBJECTID id)
		{			
			m_X				= x;
			m_Y				= y;
			m_Z				= z;
			m_ID			= id;
		}

		void		SetServerID(TYPE_OBJECTID id)	{ m_ServerID = id; }

		void		SetDelayFrame(DWORD df)		{ m_DelayFrame = df; }
	
		//-------------------------------------------------------
		// assign operator
		//-------------------------------------------------------
		virtual void	operator = (const MEffectTarget& target);		


		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void			NextPhase()				{ m_CurrentPhase++; }
		bool			IsEnd() const			{ return m_CurrentPhase>=m_MaxPhase; }
		
		
		bool			IsResultTime()	const	{ return m_bResultTime; }
		void			SetResultTime()			{ m_bResultTime = true; }

		//-------------------------------------------------------
		// Get
		//-------------------------------------------------------		
		BYTE			GetMaxPhase() const		{ return m_MaxPhase; }
		BYTE			GetCurrentPhase() const	{ return m_CurrentPhase; }
		DWORD			GetDelayFrame() const	{ return m_DelayFrame; }	
		int				GetX() const			{ return m_X; }
		int				GetY() const			{ return m_Y; }
		int				GetZ() const			{ return m_Z; }
		TYPE_OBJECTID	GetID() const			{ return m_ID; }
		TYPE_OBJECTID	GetServerID() const		{ return m_ServerID; }

		//-------------------------------------------------------		
		
		//-------------------------------------------------------		
		bool			IsResultEmpty() const					{ return m_pResult==NULL; }
		bool			IsExistResult() const					{ return m_pResult!=NULL; }
		MActionResult*	GetResult() const						{ return m_pResult; }
		void			SetResult(MActionResult* pResult);
		void			SetResultNULL()							{ m_pResult = NULL; }
		//TYPE_ACTIONINFO	GetResultActionInfo() const					{ return m_nResultActionInfo; }
		//void			SetResultActionInfo(TYPE_ACTIONINFO rai)	{ m_nResultActionInfo = rai; }
		
	public :
		BYTE			m_MaxPhase;			
		BYTE			m_CurrentPhase;		

		bool			m_bResultTime;		

		DWORD			m_DelayFrame;		

		//-------------------------------------------------------		
		
		//-------------------------------------------------------		
		int				m_X;				
		int				m_Y;
		int				m_Z;
		TYPE_OBJECTID	m_ID;
		TYPE_OBJECTID	m_ServerID;

		//-------------------------------------------------------		
		
		//-------------------------------------------------------		
		
		MActionResult*	m_pResult;				


		//-------------------------------------------------------		
		// Instance ID
		//-------------------------------------------------------		
		BYTE			m_EffectID;
		static BYTE		s_EffectID;
};

//----------------------------------------------------------------------
// MPortalEffectTarget
//----------------------------------------------------------------------
class MPortalEffectTarget : public MEffectTarget {
	public :
		MPortalEffectTarget(const MEffectTarget& target);
		MPortalEffectTarget(BYTE max);
		~MPortalEffectTarget();

		virtual EFFECT_TARGET_TYPE	GetEffectTargetType() const	{ return EFFECT_TARGET_PORTAL; }

		//------------------------------------------------------------------
		// Set
		//------------------------------------------------------------------
		void					SetOwnerName(const char* name)	{ m_OwnerName = name; }
		void					SetPortal(int id, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
		{
			m_ZoneID = id;
			m_ZoneX = x;
			m_ZoneY = y;
		}

		//------------------------------------------------------------------
		// Get
		//------------------------------------------------------------------
		const char*				GetOwnerName() const	{ return m_OwnerName.GetString(); }
		int						GetZoneID() const		{ return m_ZoneID; }
		TYPE_SECTORPOSITION		GetZoneX() const		{ return m_ZoneX; }
		TYPE_SECTORPOSITION		GetZoneY() const		{ return m_ZoneY; }
		const char*				GetZoneName() const;

		//------------------------------------------------------------------
		// assign
		//------------------------------------------------------------------
		virtual void	operator = (const MEffectTarget& target);		

	protected :
		MString					m_OwnerName;
		int						m_ZoneID;
		TYPE_SECTORPOSITION		m_ZoneX;
		TYPE_SECTORPOSITION		m_ZoneY;
};

typedef	std::list<MEffectTarget*>		EFFECTTARGET_LIST;

#endif

