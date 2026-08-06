//----------------------------------------------------------------------
// MActionInfo.h
//----------------------------------------------------------------------
//

//
//  = ActionInfoNode
//    - EffectGeneratorTableID
//    - BltType
//    - FrameID

//

//
//----------------------------------------------------------------------
 
//----------------------------------------------------------------------


#ifndef	__MACTIONINFOTABLE_H__
#define	__MACTIONINFOTABLE_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include "MTypeDef.h"
#include "DrawTypeDef.h"
#include "MActionResultDef.h"
#include "MEffectStatusDef.h"
#include "CTypeTable.h"
//#include "SkillDef.h"
#include "MString.h"
#include <fstream>
using namespace std;

//----------------------------------------------------------------------

//----------------------------------------------------------------------
#define	FLAG_ACTIONINFO_TARGET_NONE			0		
#define	FLAG_ACTIONINFO_TARGET_SELF			0x01	
#define FLAG_ACTIONINFO_TARGET_OTHER		0x02	
#define	FLAG_ACTIONINFO_TARGET_ZONE			0x04	
#define	FLAG_ACTIONINFO_TARGET_ITEM			0x08	

//----------------------------------------------------------------------

//----------------------------------------------------------------------
enum ACTIONINFO_PACKET {
	ACTIONINFO_PACKET_NONE,
	ACTIONINFO_PACKET_SELF,			
	ACTIONINFO_PACKET_OTHER,		
	ACTIONINFO_PACKET_ZONE,			
	ACTIONINFO_PACKET_ITEM,			
	ACTIONINFO_PACKET_BLOOD_DRAIN,	
	ACTIONINFO_PACKET_THROW_BOMB,	
	ACTIONINFO_PACKET_UNTRANSFORM,	
	ACTIONINFO_PACKET_VISIBLE,		
	ACTIONINFO_PACKET_ABSORB_SOUL,	
};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
#define	FLAG_ACTIONINFO_START_USER				0x01	
#define	FLAG_ACTIONINFO_START_TARGET			0x02	
#define	FLAG_ACTIONINFO_START_SKY				0x04	

//----------------------------------------------------------------------

//----------------------------------------------------------------------
#define	FLAG_ACTIONINFO_USER_ALL				0xFF	
#define	FLAG_ACTIONINFO_USER_NONCREATURE		0x01	
#define	FLAG_ACTIONINFO_USER_SLAYER				0x02	
#define	FLAG_ACTIONINFO_USER_VAMPIRE			0x04	
#define	FLAG_ACTIONINFO_USER_NPC				0x08	
#define	FLAG_ACTIONINFO_USER_MONSTER			0x10	
#define	FLAG_ACTIONINFO_USER_OUSTERS			0x20	

//----------------------------------------------------------------------

//----------------------------------------------------------------------

#define FLAG_ACTIONINFO_WEAPON_ALL				0x007F	
#define FLAG_ACTIONINFO_WEAPON_HAND				0x0001	
#define FLAG_ACTIONINFO_WEAPON_SWORD			0x0002	
#define FLAG_ACTIONINFO_WEAPON_BLADE			0x0004	
#define FLAG_ACTIONINFO_WEAPON_GUN_ALL			0x0078	// TR
#define FLAG_ACTIONINFO_WEAPON_GUN_SR			0x0008	// TR
#define FLAG_ACTIONINFO_WEAPON_GUN_SG			0x0010	// SG
#define FLAG_ACTIONINFO_WEAPON_GUN_AR			0x0020	// AR
#define FLAG_ACTIONINFO_WEAPON_GUN_SMG			0x0040	// SMG
#define FLAG_ACTIONINFO_WEAPON_SHIELD			0x0080	
#define FLAG_ACTIONINFO_WEAPON_HOLY_WATER		0x0100	
#define FLAG_ACTIONINFO_WEAPON_BOMB				0x0200	
#define FLAG_ACTIONINFO_WEAPON_CHAKRAM			0x0400	

//----------------------------------------------------------------------

//----------------------------------------------------------------------
#define	FLAG_ACTIONINFO_CURRENT_WEAPON_ACTION					0x01	
#define	FLAG_ACTIONINFO_CURRENT_WEAPON_RANGE					0x02	
#define	FLAG_ACTIONINFO_CURRENT_WEAPON_SOUND					0x04	
#define	FLAG_ACTIONINFO_CURRENT_WEAPON_CASTING_STARTFRAME		0x08	
#define	FLAG_ACTIONINFO_CURRENT_WEAPON_CASTING_FRAMES			0x10	
#define	FLAG_ACTIONINFO_CURRENT_WEAPON_DELAY					0x20	
#define	FLAG_ACTIONINFO_CURRENT_WEAPON_ACTIONEFFECTSPRITETYPE	0x40	

//----------------------------------------------------------------------
// option flag
//----------------------------------------------------------------------
#define	FLAG_ACTIONINFO_OPTION_RANGE_TO_DIRECTION	0x01	
#define FLAG_ACTIONINFO_OPTION_USE_WITH_BLESS		0x02	

//----------------------------------------------------------------------
// Select Creature
//----------------------------------------------------------------------
#define FLAG_ACTIONINFO_SELECT_ALL					0xFF
#define	FLAG_ACTIONINFO_SELECT_ENEMY				0x01
#define	FLAG_ACTIONINFO_SELECT_FRIEND				0x02
#define	FLAG_ACTIONINFO_SELECT_PARTY				0x04
#define	FLAG_ACTIONINFO_SELECT_GUILD				0x08
#define	FLAG_ACTIONINFO_SELECT_FOLLOWER				0x10

//----------------------------------------------------------------------
// main node NULL
//----------------------------------------------------------------------
#define	MAIN_NODE_NULL						0xFFFF

#define MAX_ACTION_STEP							5

//----------------------------------------------------------------------
// ACTION_INFO_NODE class
//----------------------------------------------------------------------
class ACTION_INFO_NODE {
	public :
		ACTION_INFO_NODE()
		{
			SoundID		= SOUNDID_NULL;
			//Light		= 1;
			LinkCount	= 0xFFFF;

			bDelayNode	= false;
			bResultTime = false;	
			bUseCoord = false;
		}
		
		~ACTION_INFO_NODE() {}
		

	public :
		TYPE_EFFECTGENERATORID		EffectGeneratorID;	// EffectGenerator ID		
		TYPE_EFFECTSPRITETYPE		EffectSpriteType;	// EffectSpriteType
		WORD						Step;				
		WORD						Count;				
		WORD						LinkCount;			
		TYPE_SOUNDID				SoundID;			
		
		bool						bDelayNode;			
		bool						bResultTime;		
		bool						bUseCoord;			
		
	public :
		void			SetUseCoord()				{ bUseCoord = true; }		
		void			SetDelayNode()				{ bDelayNode = true; }
		void			SetResultTime()				{ bResultTime = true; }
		void			SetCoord(BYTE x, BYTE y)	{ if( bUseCoord ) Step = (x<<8)|y; }
		BYTE			GetX()						{ if( bUseCoord ) return Step>>8; return 0; }
		BYTE			GetY()						{ if( bUseCoord ) return Step&0xff; return 0; }

		void			operator = (const ACTION_INFO_NODE& node)
		{
			EffectGeneratorID	= node.EffectGeneratorID;
			EffectSpriteType	= node.EffectSpriteType;
			Step				= node.Step;
			Count				= node.Count;
			LinkCount			= node.LinkCount;
			SoundID				= node.SoundID;
			//Light				= node.Light;
			bResultTime			= node.bResultTime;
			bUseCoord			= node.bUseCoord;
		}

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void			SetChildMode();

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);
		void			LoadFromFile(std::ifstream& file);
};


//----------------------------------------------------------------------


//----------------------------------------------------------------------
class MActionInfo : public CTypeTable<ACTION_INFO_NODE> {
	public :
		MActionInfo();	
		~MActionInfo();

		//------------------------------------------------------
		// Set
		//------------------------------------------------------
		void	Set(const char* name, BYTE action, TYPE_EFFECTSPRITETYPE ActionEffectSpriteType, BYTE range, BYTE fTarget, BYTE fStart=FLAG_ACTIONINFO_START_USER, WORD delay=0, int value=0)
		{
			m_Name						= name;
			m_Action					= action;
			m_ActionEffectSpriteType	= ActionEffectSpriteType;
			m_Range						= range;
			m_fTarget					= fTarget;
			m_fStart					= fStart;
			m_Delay						= delay;
			m_Value						= value;			
		}

		//------------------------------------------------------
		// female
		//------------------------------------------------------
		TYPE_EFFECTSPRITETYPE	GetActionEffectSpriteTypeFemale() const	{ return m_ActionEffectSpriteTypeFemale; }
		void		SetActionEffectSpriteTypeFemale(TYPE_EFFECTSPRITETYPE et)
		{
			m_ActionEffectSpriteTypeFemale = et; 
		}

		//------------------------------------------------------
		
		//------------------------------------------------------
		bool		IsCastingEffectToSelf() const			{ return m_bCastingEffectToSelf; }
		void		SetCastingEffectToSelf()				{ m_bCastingEffectToSelf = true; }
		void		SetCastingEffectToOther()				{ m_bCastingEffectToSelf = false; }

		
		void		SetCastingStartFrameAll(int f)			{ m_CastingStartFrame[0] = m_CastingStartFrame[1] = m_CastingStartFrame[2] = f; }
		void		SetCastingFramesAll(int f)				{ m_CastingFrames[0] = m_CastingFrames[1] = m_CastingFrames[2] = f; }

		void		SetCastingStartFrame(int speed, int f)			{ m_CastingStartFrame[speed] = f; }
		void		SetCastingFrames(int speed, int f)				{ m_CastingFrames[speed] = f; }

		int			GetCastingStartFrame(int speed) const	{ return m_CastingStartFrame[speed]; }
		int			GetCastingFrames(int speed) const		{ return m_CastingFrames[speed]; }
		
		void		SetStartWithCasting()	{ m_StartFrame[0] = m_StartFrame[1] = m_StartFrame[2] = 0; }
		void		SetStartAfterCasting()	{ m_StartFrame[0] = m_StartFrame[1] = m_StartFrame[2] = 0xFFFF; }

		
		void		SetCastingActionInfo(TYPE_ACTIONINFO ai)	{ m_CastingActionInfo=ai; }		
		TYPE_ACTIONINFO	GetCastingActionInfo() const		{ return m_CastingActionInfo; }		

		
		bool		IsCastingAction() const					{ return m_bCastingAction; }		
		void		SetCastingAction()						{ m_bCastingAction=true; }
		void		UnSetCastingAction()					{ m_bCastingAction=false; }	

		
		

		//------------------------------------------------------
		// Sound ID
		//------------------------------------------------------
		void	SetSoundID(TYPE_SOUNDID id)			{ m_SoundID = id; }

		//------------------------------------------------------
		// Main Node
		//------------------------------------------------------
		void	SetMainNode(int n)					{ m_MainNode=n; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		void		SetAttack()				{ m_bAttack = TRUE; }
		void		UnSetAttack()			{ m_bAttack = FALSE; }
		BOOL		IsAttack() const		{ return m_bAttack; }

		//------------------------------------------------------
		// get
		//------------------------------------------------------
		const char*	GetName() const			{ return m_Name; }
		BYTE		GetAction() const		{ return m_Action; }
		BYTE		GetRange() const		{ return m_Range; }
		TYPE_EFFECTSPRITETYPE	GetActionEffectSpriteType() const { return m_ActionEffectSpriteType; }
		WORD		GetDelay() const		{ return m_Delay; }
		int			GetValue() const		{ return m_Value; }
		TYPE_SOUNDID	GetSoundID() const		{ return m_SoundID; }
		BOOL		HasMainNode() const		{ return m_MainNode!=MAIN_NODE_NULL; }
		int			GetMainNode() const		{ return m_MainNode; }
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		BYTE		GetTarget() const		{ return m_fTarget; }
		bool		IsTargetNone() const	{ return m_fTarget==0; }
		BYTE		IsTargetSelf() const	{ return m_fTarget & FLAG_ACTIONINFO_TARGET_SELF; }
		BYTE		IsTargetOther() const	{ return m_fTarget & FLAG_ACTIONINFO_TARGET_OTHER; }
		BYTE		IsTargetZone() const	{ return m_fTarget & FLAG_ACTIONINFO_TARGET_ZONE; }
		BYTE		IsTargetItem() const	{ return m_fTarget & FLAG_ACTIONINFO_TARGET_ITEM; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		void		SetPacketType(ACTIONINFO_PACKET ap)		{ m_PacketType = ap; }
		ACTIONINFO_PACKET	GetPacketType() const			{ return m_PacketType; }
		bool		IsPacketTypeNone() const	{ return m_PacketType == ACTIONINFO_PACKET_NONE; }
		BYTE		IsPacketTypeSelf() const	{ return m_PacketType == ACTIONINFO_PACKET_SELF; }
		BYTE		IsPacketTypeOther() const	{ return m_PacketType == ACTIONINFO_PACKET_OTHER; }
		BYTE		IsPacketTypeZone() const	{ return m_PacketType == ACTIONINFO_PACKET_ZONE; }
		BYTE		IsPacketTypeItem() const	{ return m_PacketType == ACTIONINFO_PACKET_ITEM; }
		BYTE		IsPacketTypeThrowBomb() const	{ return m_PacketType == ACTIONINFO_PACKET_THROW_BOMB; }
		BYTE		IsPacketTypeUntransform() const	{ return m_PacketType == ACTIONINFO_PACKET_UNTRANSFORM; }
		BYTE		IsPacketTypeVisible() const		{ return m_PacketType == ACTIONINFO_PACKET_VISIBLE; }


		//------------------------------------------------------
		
		//------------------------------------------------------
		BYTE		GetStart() const		{ return m_fStart; }
		bool		IsStartNone() const		{ return m_fStart==0; }
		BYTE		IsStartUser() const		{ return m_fStart & FLAG_ACTIONINFO_START_USER; }
		BYTE		IsStartTarget() const	{ return m_fStart & FLAG_ACTIONINFO_START_TARGET; }
		BYTE		IsStartSky() const		{ return m_fStart & FLAG_ACTIONINFO_START_SKY; }
		
		void		SetStartFrameAll(int f)	{ m_StartFrame[0] = m_StartFrame[1] = m_StartFrame[2] = f; }
		void		SetStartFrame(int speed, int f)	{ m_StartFrame[speed] = f; }
		int			GetStartFrame(int speed) const	{ return m_StartFrame[speed]; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		void	SetUser(BYTE fUser)				{ m_fUserType=fUser; }
		BYTE	GetUser() const					{ return m_fUserType; }
		void	AddUserNonCreature() 			{ m_fUserType|=FLAG_ACTIONINFO_USER_NONCREATURE; }
		void	AddUserSlayer() 				{ m_fUserType|=FLAG_ACTIONINFO_USER_SLAYER; }
		void	AddUserVampire() 				{ m_fUserType|=FLAG_ACTIONINFO_USER_VAMPIRE; }
		void	AddUserOusters() 				{ m_fUserType|=FLAG_ACTIONINFO_USER_OUSTERS; }
		void	AddUserNPC() 					{ m_fUserType|=FLAG_ACTIONINFO_USER_NPC; }
		void	AddUserMonster()				{ m_fUserType|=FLAG_ACTIONINFO_USER_MONSTER; }
		BYTE	IsUserNonCreature() const		{ return m_fUserType & FLAG_ACTIONINFO_USER_NONCREATURE; }
		BYTE	IsUserSlayer() const			{ return m_fUserType & FLAG_ACTIONINFO_USER_SLAYER; }
		BYTE	IsUserVampire() const			{ return m_fUserType & FLAG_ACTIONINFO_USER_VAMPIRE; }
		BYTE	IsUserOusters() const			{ return m_fUserType & FLAG_ACTIONINFO_USER_OUSTERS; }
		BYTE	IsUserNPC() const				{ return m_fUserType & FLAG_ACTIONINFO_USER_NPC; }
		BYTE	IsUserMonster() const			{ return m_fUserType & FLAG_ACTIONINFO_USER_MONSTER; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		WORD	IsWeaponTypeAll() const			{ return (m_fWeaponType & FLAG_ACTIONINFO_WEAPON_ALL)==FLAG_ACTIONINFO_WEAPON_ALL; }
		WORD	IsWeaponTypeGunAll() const		{ return (m_fWeaponType & FLAG_ACTIONINFO_WEAPON_GUN_ALL)==FLAG_ACTIONINFO_WEAPON_GUN_ALL; }
		WORD	IsWeaponTypeGunAny() const		{ return m_fWeaponType & FLAG_ACTIONINFO_WEAPON_GUN_ALL; }
		WORD	GetWeaponType() const			{ return m_fWeaponType; }
		void	SetWeaponType(WORD flag)		{ m_fWeaponType = flag; }
		void	AddWeaponType(WORD flag)		{ m_fWeaponType |= flag; }		

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		BYTE	IsAffectCurrentWeaponAction() const	{ return m_fCurrentWeapon & FLAG_ACTIONINFO_CURRENT_WEAPON_ACTION; }
		BYTE	IsAffectCurrentWeaponRange() const	{ return m_fCurrentWeapon & FLAG_ACTIONINFO_CURRENT_WEAPON_RANGE; }
		BYTE	IsAffectCurrentWeaponSound() const	{ return m_fCurrentWeapon & FLAG_ACTIONINFO_CURRENT_WEAPON_SOUND; }
		BYTE	IsAffectCurrentWeaponCastingStartFrame() const	{ return m_fCurrentWeapon & FLAG_ACTIONINFO_CURRENT_WEAPON_CASTING_STARTFRAME; }
		BYTE	IsAffectCurrentWeaponCastingFrames() const	{ return m_fCurrentWeapon & FLAG_ACTIONINFO_CURRENT_WEAPON_CASTING_FRAMES; }
		BYTE	IsAffectCurrentWeaponDelay() const	{ return m_fCurrentWeapon & FLAG_ACTIONINFO_CURRENT_WEAPON_DELAY; }
		BYTE	IsAffectCurrentWeaponActionEffectSpriteType() const	{ return m_fCurrentWeapon & FLAG_ACTIONINFO_CURRENT_WEAPON_ACTIONEFFECTSPRITETYPE; }
		int		GetAffectCurrentWeaponActionInfoPlus() const	{ return m_PlusActionInfo; }
		void	SetAffectCurrentWeaponAction()		{ m_fCurrentWeapon |= FLAG_ACTIONINFO_CURRENT_WEAPON_ACTION; }
		void	SetAffectCurrentWeaponRange()		{ m_fCurrentWeapon |= FLAG_ACTIONINFO_CURRENT_WEAPON_RANGE; }
		void	SetAffectCurrentWeaponSound()		{ m_fCurrentWeapon |= FLAG_ACTIONINFO_CURRENT_WEAPON_SOUND; }
		void	SetAffectCurrentWeaponCastingStartFrame()	{ m_fCurrentWeapon |= FLAG_ACTIONINFO_CURRENT_WEAPON_CASTING_STARTFRAME; }
		void	SetAffectCurrentWeaponCastingFrames()		{ m_fCurrentWeapon |= FLAG_ACTIONINFO_CURRENT_WEAPON_CASTING_FRAMES; }
		void	SetAffectCurrentWeaponDelay()		{ m_fCurrentWeapon |= FLAG_ACTIONINFO_CURRENT_WEAPON_DELAY; }
		void	SetAffectCurrentWeaponActionEffectSpriteType()		{ m_fCurrentWeapon |= FLAG_ACTIONINFO_CURRENT_WEAPON_ACTIONEFFECTSPRITETYPE; }
		void	SetAffectCurrentWeaponActionInfoPlus(int pai)	{ m_PlusActionInfo = pai; }
				
		//-------------------------------------------------------
		// option flag
		//-------------------------------------------------------
		BYTE	HasOption() const					{ return m_fOption; }
		BYTE	IsOptionRangeToDirection() const	{ return m_fOption & FLAG_ACTIONINFO_OPTION_RANGE_TO_DIRECTION; }
		BYTE	IsOptionUseWithBless() const		{ return m_fOption & FLAG_ACTIONINFO_OPTION_USE_WITH_BLESS; }
		void	SetOptionRangeToDirection()			{ m_fOption |= FLAG_ACTIONINFO_OPTION_RANGE_TO_DIRECTION; }
		void	SetOptionUseWithBless()				{ m_fOption |= FLAG_ACTIONINFO_OPTION_USE_WITH_BLESS; }
		

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		
		//
		//				id						value
		//
		
		//-------------------------------------------------------
		void	SetActionResult(TYPE_ACTIONRESULTID id, int value=0)	{ m_ActionResultID = id; m_ActionResultValue = value; }		
		TYPE_ACTIONRESULTID		GetActionResultID() const				{ return m_ActionResultID; }
		int		GetActionResultValue() const							{ return m_ActionResultValue; }

		//-------------------------------------------------------
		// EffectStatus
		//-------------------------------------------------------
		void			SetEffectStatus(EFFECTSTATUS effectStatus)		{ m_EffectStatus = effectStatus; }
		EFFECTSTATUS	GetEffectStatus() const							{ return m_EffectStatus; }

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		int				IsUseRepeatFrame() const							{ return m_bUseRepeatFrame; }
		void			UseRepeatFrame()									{ m_bUseRepeatFrame = true; }
		void			SetRepeatFrame(int speed, int start, int frames)			{ m_RepeatStartFrame[speed] = start; m_RepeatEndFrame[speed] = start+frames-1; }
		int				GetRepeatStartFrame(int speed) const						{ return m_RepeatStartFrame[speed]; }
		int				GetRepeatEndFrame(int speed) const						{ return m_RepeatEndFrame[speed]; }

		void			SetRepeatLimit(WORD limit)							{ m_RepeatLimit = limit; }
		WORD			GetRepeatLimit() const								{ return m_RepeatLimit; }
		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void			SetChildMode();
		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void		SetSelectCreatureAll()				{ m_fSelectCreature = FLAG_ACTIONINFO_SELECT_ALL; }
		void		SetSelectCreature(BYTE fSelect)		{ m_fSelectCreature = fSelect; }
		BYTE		GetActionTarget() const				{ return m_fSelectCreature; }

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);

		//-------------------------------------------------------
		// Grade 
		//-------------------------------------------------------
		void		SetUseActionGrade() {	m_bUseGrade = true; }
		void		UnsetUseActionGrade() { m_bUseGrade = false; }
		bool		IsUseActionGrade() { return m_bUseGrade; }

		//-------------------------------------------------------
		// ActionStep
		//-------------------------------------------------------
		void		SetUseActionStep()	{ m_bUseActionStep = true; }
		void		SetActionStep(BYTE step,  TYPE_ACTIONINFO action);
		bool		IsUseActionStep() { return m_bUseActionStep; }
		TYPE_ACTIONINFO	GetActionStep(BYTE step);

		void		SetParentActionInfo( TYPE_ACTIONINFO Parent );
		TYPE_ACTIONINFO	GetParentActionInfo();

		//-------------------------------------------------------
		// ForceSelectEffectTarget
		//-------------------------------------------------------
		void		SetAttachSelf()	{ m_bAttachSelf = true; }
		bool		IsAttachSelf() { return m_bAttachSelf; }

		//-------------------------------------------------------
		// MasterySkill
		//-------------------------------------------------------
		void		SetMasterySkillStep(BYTE step = 3)	{ m_MasterySkillStep = step; }
		bool		IsMasterySkillStep() { return m_MasterySkillStep!=0; }
		BYTE		GetMasterySkillStep() { return m_MasterySkillStep; }

		bool		IsIgnoreSkillFailDelay() { return m_bIgnoreFailDelay;}
		void		SetSkillFailDelay(bool bflag = true) {	m_bIgnoreFailDelay = bflag;}
	protected :
		MString						m_Name;						
		BYTE						m_Action;					

		bool						m_bUseActionStep;
		TYPE_ACTIONINFO				m_ActionStep[MAX_ACTION_STEP];			
		TYPE_ACTIONINFO				m_Parent;				

		
		TYPE_ACTIONINFO				m_CastingActionInfo;		
		bool						m_bCastingAction;			
		TYPE_EFFECTSPRITETYPE		m_ActionEffectSpriteType;	// Casting EffectSpriteType		
		TYPE_EFFECTSPRITETYPE		m_ActionEffectSpriteTypeFemale;	
		int							m_CastingStartFrame[3];		
		int							m_CastingFrames[3];			
		int							m_PlusActionInfo;			
		bool						m_bCastingEffectToSelf;		
		
		
		BYTE						m_Range;					
		BYTE						m_fTarget;					
		BYTE						m_fStart;					
		BYTE						m_fUserType;				
		WORD						m_fWeaponType;				
		BYTE						m_fCurrentWeapon;			

		ACTIONINFO_PACKET			m_PacketType;				

		WORD						m_Delay;					
		int							m_Value;					
		TYPE_SOUNDID				m_SoundID;					
		int							m_MainNode;					

		BOOL						m_bAttack;					

		
		int							m_StartFrame[3];				
		
		TYPE_ACTIONRESULTID			m_ActionResultID;			
		int							m_ActionResultValue;		

		BYTE						m_fOption;

		EFFECTSTATUS				m_EffectStatus;			

		
		bool						m_bUseRepeatFrame;
		int							m_RepeatStartFrame[3];
		int							m_RepeatEndFrame[3];
		WORD						m_RepeatLimit;			

		
		BYTE						m_fSelectCreature;
		bool						m_bUseGrade;		
		bool						m_bAttachSelf;
		
		BYTE						m_MasterySkillStep;
		bool						m_bIgnoreFailDelay; 
		
		
		bool						m_bAdvancementClassSkill;		
		bool						m_bNonAdvancementClassSkill;	
};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MActionInfoTable : public CTypeTable<MActionInfo> {
	public :
		MActionInfoTable();
		~MActionInfoTable();

		//---------------------------------------------
		// Set/Get Min Result ActionInfo
		//---------------------------------------------
		void	SetMinResultActionInfo(DWORD nActionInfo)	{ m_nMinResultActionInfo = nActionInfo; }
		DWORD	GetMinResultActionInfo() const				{ return m_nMinResultActionInfo; }

		//---------------------------------------------
		
		//---------------------------------------------
		void	SetMaxResultActionInfo(DWORD nActionInfo)	{ m_nMaxResultActionInfo = nActionInfo; }
		DWORD	GetMaxResultActionInfo() const				{ return m_nMaxResultActionInfo; }

		void	SetMinStepActionInfo(DWORD nActionInfo) {m_nMinStepActionInfo = nActionInfo; }
		DWORD	GetMinStepActionInfo()	const				{ return m_nMinResultActionInfo; }

		bool	IsResultActionInfo(TYPE_ACTIONINFO type);
		bool	IsActionInfo(TYPE_ACTIONINFO type);

		TYPE_ACTIONINFO	GetResultActionInfo(TYPE_ACTIONINFO action);

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void			SetChildMode();

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);

	protected :
		
		DWORD			m_nMinResultActionInfo;
		DWORD			m_nMaxResultActionInfo;		
		DWORD			m_nMinStepActionInfo;
};


extern	MActionInfoTable*			g_pActionInfoTable;


#endif
