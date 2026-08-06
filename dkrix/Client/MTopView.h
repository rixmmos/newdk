//----------------------------------------------------------------------
// MTopView.h
//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
//


//


//




//
//---------------------------------------------------------------------- 
//



//




// 


//

//
//---------------------------------------------------------------------- 
//

//

//   Tile SpriteSet
//   Obstacle SpriteSet 
//   Obstacle FrameSet
//   ImageObject SpriteSet
//
//---------------------------------------------------------------------- 
//
// ImageObject Map...
//


//
//---------------------------------------------------------------------- 

#ifndef	__MTOPVIEW_H__
#define	__MTOPVIEW_H__

#pragma warning(disable:4786)

#include "MViewDef.h"
#include <fstream>
#include <map>
#include <queue>

using namespace std;

//----------------------------------------------------------------------
//
// using class..
//
//----------------------------------------------------------------------
#include "MZone.h"
class MObject;
#include "MImageObject.h"
#include "MCreature.h"
class MMissile;
class MItem;
class MEffect;
#include "SpriteLib/SP.h"
#include "FR.h"
//#include "2D.h"
#include "CSpriteTexturePartManager.h"
#include "CTexturePartManager.h"
#include "CShadowPartManager.h"
#include "DrawTextNode.h"
#include "DrawItemNameNode.h"
#include "COrderedList.h"
#include "MRequestMode.h"
#include "SpriteLib/CIndexSpritePack.h"
#include "SpriteLib/CFilter.h"
#include "SpriteLib/CFilterPack.h"
#include "SpriteLib/CSpriteOutlineManager.h"

// TileRenderer for unified tile rendering
#include "TileRenderer.h"
#include "MZoneTileProvider.h"
#include "EffectResourceContainer.h"

#ifdef PLATFORM_WINDOWS
class CD3DTextureEffect {
	public:
		void DrawEffect2D(RECT*) {}
};
#endif


//class MZoneInfo;
class TextComparison {
	public :
		
		
		
		bool operator () (DRAWTEXT_NODE * left, DRAWTEXT_NODE * right) const;
};

typedef void (*DrawCreatureExceptionProc)( MCreature* pCreature, int& action, int& frame, int& direction );

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MTopView : public MRequestMode {
	public :	
		
		typedef std::map<QWORD, MImageObject*>			IMAGEOBJECT_OUTPUT_MAP;

		
		typedef std::map<QWORD, MCreature*>				CREATURE_OUTPUT_MAP;

		
		typedef std::map<QWORD, MItem*>					ITEM_OUTPUT_MAP;

		
		//typedef	std::list<DRAWTEXT_NODE*>				DRAWTEXT_LIST;
		typedef std::priority_queue<DRAWTEXT_NODE*, std::vector<DRAWTEXT_NODE*>, TextComparison>	DRAWTEXT_PQ;

		
		typedef	std::list<DRAWITEMNAME_NODE*>			DRAWITEMNAME_LIST;

		// int ordered list
		typedef	COrderedList<int>						INT_ORDERED_LIST;

	public :
		MTopView();
		~MTopView();

		//------------------------------------------------------
		//
		// init
		//
		//------------------------------------------------------
		bool		Init();
		bool		InitChanges();	
		bool		IsInit() const		{ return m_bInit; }
		void		Release();
		void		SetSurface(CSpriteSurface*& pSurface);
		void		SetZone(MZone* pZone);
		void		RestoreSurface();
		void		ClearShadowManager();
		const FRAME_ARRAY&	GetCreatureFrameArray(int body, int action, int direction) const	{ return m_CreatureFPK[body][action][direction]; }
		const CIndexSprite*	GetCreatureSprite(int spriteID)		{ return &m_CreatureSPK[ spriteID ]; }

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------		
		void		LoadMinimap(const char* filename);//, MZoneInfo* pZoneInfo=NULL);
		bool		LoadFromFileTileSPKLargeZone(ifstream & file);	
		bool		LoadFromFileImageObjectSPKLargeZone(ifstream & file);	
		bool		LoadFromFileTileSPKSmallZone(ifstream & file);	
		bool		LoadFromFileImageObjectSPKSmallZone(ifstream & file);	
		void		LoadFromFileCreatureSPK(int n);			// creature load		
// 		void		LoadFromFileAddonSPK(int frame, int action);		// addon load		
//		void		LoadFromFileCreatureActionSPK(int frame, int action);		// addon load		
		bool		LoadFromFileTileAndImageObjectSet(const CSpriteSetManager &TileSSM, const CSpriteSetManager &ImageObjectSSM);
		void		ReleaseCreatureSPK(int n);			// creature release
		void		ReleaseTileSPKLargeZone();
		void		ReleaseImageObjectSPKLargeZone();
		void		ReleaseTileSPKSmallZone();
		void		ReleaseImageObjectSPKSmallZone();
		//void		ReleaseAddonSPK(int frame, int action);
//		void		ReleaseCreatureActionSPK(int frame, int action);

		void		ReleaseUselessCreatureSPKExcept(const INT_ORDERED_LIST& listUse);
		void		AddMonsterSpriteTypes(const INT_ORDERED_LIST& liseUse);

//		void		StopLoadImageObjectSPK()	{ m_ImageObjectSPK.SetLoadingStop(); }
//		void		StopLoadTileSPK()			{ m_TileSPK.SetLoadingStop(); }

		
		void		SetFirstDraw()			{ m_bFirstTileDraw = true; }
		
		//------------------------------------------------------
		//
		// Draw Functions
		//
		//------------------------------------------------------
		void		Draw(int firstPointX, int firstPointY);

		
		void		DrawDebugInfo(void* pSurface);

		void		DrawTitleEffect(POINT* pPoint);

		void		DrawBloodBibleEffect_InGear(POINT* pPoint);
		
		//------------------------------------------------------
		//
		// Fade In/out
		//
		//------------------------------------------------------
		bool			IsFade() const			{ return m_bFade; }
		void			SetFadeStart(char start, char end, char step, BYTE r=0, BYTE g=0, BYTE b=0, WORD delay = 0);
		void			SetFadeEnd() 			{ m_bFade = false; }
		
		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		static POINT	PixelToMap(const int& Xp, const int& Yp)	{ POINT p; p.x=Xp/TILE_X; p.y=Yp/TILE_Y; return p; }//{ POINT p; p.x=Xp>>6; p.y=Yp>>5; return p; }
		static POINT	MapToPixel(const int& Xm, const int& Ym)	{ POINT p; p.x=Xm*TILE_X; p.y=Ym*TILE_Y; return p; }//{ POINT p; p.x=Xm<<6; p.y=Ym<<5; return p; }

		static int		PixelToMapX(const int& Xp)		{ return (Xp/TILE_X); }
		static int		PixelToMapY(const int& Yp)		{ return (Yp/TILE_Y); }
		static int		MapToPixelX(const int& Xm)		{ return (Xm*TILE_X); }	
		static int		MapToPixelY(const int& Ym)		{ return (Ym*TILE_Y); }

		static BYTE		GetDirectionToPosition(int originX, int originY, int destX, int destY);
		static POINT	GetChangeValueToDirection(int direction);

		//------------------------------------------------------
		
		//------------------------------------------------------
		//static POINT	PixelToMap(const int& Xp, const int& Yp)	{ POINT p; p.x=Xp>>6; p.y=Yp>>5; return p; }
		//static POINT	MapToPixel(const int& Xm, const int& Ym)	{ POINT p; p.x=Xm<<6; p.y=Ym<<5; return p; }

		//static int		PixelToMapX(const int& Xp)		{ return (Xp >> 6); }
		//static int		PixelToMapY(const int& Yp)		{ return (Yp >> 5); }
		//static int		MapToPixelX(const int& Xm)		{ return (Xm << 6); }	
		//static int		MapToPixelY(const int& Ym)		{ return (Ym << 5); }


		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		
		POINT			ScreenToPixel(int x, int y);

		
		POINT			MapToScreen(int sX, int sY);
		POINT			PixelToScreen(int x, int y);
		POINT			GetFirstZonePixel()	const		{ return m_FirstZonePixel; }		
		POINT			GetFirstSector() const			{ return m_FirstSector; }

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		// Selected Sector
		inline void		SetSelectedSector(int sX, int sY)	{ m_SelectSector.x = sX; m_SelectSector.y = sY; }	
		inline void		SetSelectedSector(const POINT& s)	{ m_SelectSector.x = s.x; m_SelectSector.y = s.y; }
		void			SetSelectedSectorNULL()		{ m_SelectSector.x = m_SelectSector.y = SECTORPOSITION_NULL; }
		POINT			GetSelectedSector(int x, int y);// const;

		
		//void			SetSelectModeAttack()				{ m_bSelectModeAttack = true; }
		//void			SetSelectModeNormal()				{ m_bSelectModeAttack = false; }


		
		MObject*		GetSelectedObject(int x, int y);
		MObject*		GetSelectedObjectSprite(int x, int y);

		
//		void			SetSelectedCreature(TYPE_OBJECTID id)	{ m_SelectCreatureID = id; m_SelectItemID = m_SelectInteractionObjectID = m_SelectEffectID = OBJECTID_NULL; }		
		void			SetSelectedCreature(TYPE_OBJECTID id)	{ m_SelectCreatureID = id; m_SelectItemID = m_SelectEffectID = OBJECTID_NULL; }		
		TYPE_OBJECTID	GetSelectedCreature() const				{ return m_SelectCreatureID; }

		
//		void			SetSelectedItem(TYPE_OBJECTID id)	{ m_SelectItemID = id;  m_SelectCreatureID = m_SelectInteractionObjectID = m_SelectEffectID = OBJECTID_NULL; }
		void			SetSelectedItem(TYPE_OBJECTID id)	{ m_SelectItemID = id;  m_SelectCreatureID = m_SelectEffectID = OBJECTID_NULL; }
		TYPE_OBJECTID	GetSelectedItemID()					{ return m_SelectItemID; }
		
		
//		void			SetSelectedInteractionObject(TYPE_OBJECTID id)	{ m_SelectInteractionObjectID = id; m_SelectItemID = m_SelectCreatureID = m_SelectEffectID = OBJECTID_NULL; }

		
//		void			SetSelectedEffect(TYPE_OBJECTID id)		{ m_SelectEffectID = id; m_SelectItemID = m_SelectCreatureID = m_SelectInteractionObjectID = OBJECTID_NULL; }
		void			SetSelectedEffect(TYPE_OBJECTID id)		{ m_SelectEffectID = id; m_SelectItemID = m_SelectCreatureID =  OBJECTID_NULL; }
		
		
//		void			SetSelectedNULL()				{ m_SelectCreatureID = m_SelectItemID = m_SelectInteractionObjectID = m_SelectEffectID = OBJECTID_NULL;}
		void			SetSelectedNULL()				{ m_SelectCreatureID = m_SelectItemID = m_SelectEffectID = OBJECTID_NULL;}


		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------		
		BYTE			GetDarkBits() const				{ return m_DarkBits; }
		void			SetDarkBits(BYTE DarkBits)		{ if (DarkBits<16) m_DarkBits = DarkBits; }

		
		void			ClearLightBufferFilter3D();
		void			AddLightFilter3D(int x, int y, BYTE range, bool bMapPixel=true, bool bForceLight=false);
		void			DrawLightBuffer3D();

		
		void			ClearLightBufferFilter2D();
		void			AddLightFilter2D(int x, int y, BYTE range, bool bMapPixel=true, bool bForceLight=false);
		void			DrawLightBuffer2D();		

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		QWORD			GetOutputImageObjectID(const MImageObject* pImageObject) const
											{ return ((QWORD)pImageObject->GetViewpoint() << 32) | pImageObject->GetID(); }

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------		
		
		QWORD			GetOutputCreatureID(const MCreature* pCreature) const	
											{ return ((QWORD)pCreature->GetPixelY() << 32) | pCreature->GetID(); }

		QWORD			GetOutputCreatureID(TYPE_OBJECTID id, int y) const	
											{ return ((QWORD)y << 32) | id; }

		
		void			AddOutputCreatureAll();

		
		bool			AddOutputCreature(MCreature* pCreature);

		
		void			ClearOutputCreature()	{ m_mapCreature.clear(); }

		
		bool			RemoveOutputCreature(MCreature* pCreature);

		
		bool			UpdateOutputCreature(TYPE_OBJECTID id, int y0, int y1);


		//------------------------------------------------------		
		//
		
		//
		//------------------------------------------------------		
		void			ClearTextList();
		void			AddText(DRAWTEXT_NODE* pNode);
		void			DrawTextList();

		//------------------------------------------------------		
		//
		
		//
		//------------------------------------------------------		
		bool			IsDrawItemNameList() const		{ return m_bDrawItemNameList; }
		void			SetDrawItemNameList()			{ m_bDrawItemNameList = true; }
		void			UnSetDrawItemNameList()			{ m_bDrawItemNameList = false; }
		void			ClearItemNameList();
		void			AddItemName(DRAWITEMNAME_NODE* pNode);
		void			DrawItemNameList();
		MItem*			SelectItemName(int x, int y);

		//------------------------------------------------------		
		//
		// Request
		//
		//------------------------------------------------------		
		bool			IsDrawRequest() const			{ return m_bDrawRequest; }
		
		//------------------------------------------------------		
		// Get MaxEffectFrame
		//------------------------------------------------------		
		int				GetMaxEffectFrame(BLT_TYPE bltType, TYPE_FRAMEID frameID) const;
		int				GetEffectLight(BLT_TYPE bltType, TYPE_FRAMEID frameID, int dir, int frame) const;
		int				GetEffectSpriteType(BLT_TYPE bltType, TYPE_FRAMEID frameID) const;

		//------------------------------------------------------		
		
		//------------------------------------------------------		
		int				GetRandomMonsterTypeInZone() const;



	protected :
		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		bool			InitCreatureFrames();		
		bool			InitImageFrames();			
		bool			InitAnimationFrames();		
		bool			InitEffectFrames();			
		bool			InitSprites();				
		bool			InitSurfaces();				
		bool			InitFilters();				
		
		bool			InitColors();				
// 		bool			Init3DBoxSurface();
		bool			InitFonts();				

		
// 		void			Set3DBoxColor(WORD pixel);

		//------------------------------------------------------
		
		//------------------------------------------------------
		void			DetermineImageObject();
		void			UpdateImageObject(const POINT &newFirstSector);

		//------------------------------------------------------
		// SurfaceLock
		//------------------------------------------------------
		//BOOL			SurfaceLock();
		//BOOL			SurfaceLockForEffect();
		//void			SurfaceLockRestore( BOOL bOldLock );

		//------------------------------------------------------
		
		//------------------------------------------------------
		void		DrawTileSurface();
		void		DrawZone(int X,int Y);
		void		DrawInformation();		
		void		DrawEventString(int &strX, int &strY);			
		int			DrawChatString(POINT* pPoint, MCreature* pCreature, COLORREF color, BYTE flag=0);
		void		DrawItem(POINT* pPoint, MItem* pItem);
		void		DrawItemShadow(POINT* pPoint, MItem* pItem);
		void		DrawImageObject(POINT* pPoint, MImageObject* pImageObject);
		void		DrawAttachEffect(POINT* pPoint, ATTACHEFFECT_LIST::const_iterator iEffect, BYTE size, MCreature* pCreature, int type=0);
		void		DrawGroundEffect();
		void		DrawEffect(POINT* pPoint, MEffect* pEffect, bool bSeletable);
		void		DrawEffect(POINT* pPoint, EFFECT_LIST::const_iterator iEffect, BYTE size);
		void		DrawAlphaBox(RECT* pRect, BYTE r, BYTE g, BYTE b, BYTE alpha);
		void		DrawTestHelp();
		void		DrawInventoryEffect(POINT* pPoint);
		void		DrawMinimap(int x, int y, BYTE scale);
		void		DrawItemBroken(int x, int y);
		void		DrawPartyHP(POINT* pPoint, MCreature* pCreature);
// 		void		DrawBox3D(RECT* pRect, WORD pixel);			
		void		DrawFade();

		//------------------------------------------------------
		
		//------------------------------------------------------
		void		DrawCreature(POINT* pPoint, MCreature* pCreature);		
		void		DrawCreatureShadow(POINT* pPoint, MCreature* pCreature);
		void		DrawCreatureName(MCreature* pCreature);
		void		DrawCreatureMyName();
		void		DrawCreatureHPModify(POINT *point, MCreature* pCreature);
		void		DrawUndergroundCreature(POINT *pPoint, MCreature *pCreature);

		void		DrawGuildMarkInSiegeWar(MCreature* pCreature, int YPos); 

	protected :		
		bool				m_bInit;

		//------------------------------------------------------
		
		//------------------------------------------------------
		CSpriteSurface*		m_pSurface;
		//S_SURFACEINFO		m_SurfaceInfo;
	

		// SpriteSurfacePack
		
		//CSpriteSurface**			m_ppSurface;			// SpriteSurfaces

		//------------------------------------------------------
		//
		//                   Frame
		//
		//------------------------------------------------------
	public :
		//------------------------------------------------------
		// FramePacks
		//------------------------------------------------------		
		CCreatureFramePack		m_CreatureFPK;			// Creature frames
		CCreatureFramePack		m_AddonFPK;				// Player addon frames
		CCreatureFramePack		m_OustersFPK;			// Ousters frames

		CCreatureFramePack		m_AdvancementSlayerManFPK;			// Creature frames
		CCreatureFramePack		m_AdvancementSlayerWomanFPK;			// Creature frames
		CCreatureFramePack		m_AdvancementVampireManFPK;				// Player addon frames
		CCreatureFramePack		m_AdvancementVampireWomanFPK;				// Player addon frames
		CCreatureFramePack		m_AdvancementOustersFPK;			// Ousters frames

		CImageFramePack			m_ItemTileFPK;			
		CAnimationFramePack		m_ItemDropFPK;			
		CAnimationFramePack		m_ItemBrokenFPK;		
		CAnimationFramePack		m_ImageObjectFPK;		
		CAnimationFramePack		m_ImageObjectShadowFPK;		

		CEffectFramePack		m_EffectAlphaFPK;		
		CEffectFramePack		m_EffectNormalFPK;		
		CEffectFramePack		m_EffectShadowFPK;		
		CEffectFramePack		m_EffectScreenFPK;
		
		CCreatureFramePack		m_CreatureShadowFPK;			// Creature frames
		CCreatureFramePack		m_AddonShadowFPK;			// Player addon frames
		CCreatureFramePack		m_OustersShadowFPK;			// Ousters addon frames

		CCreatureFramePack		m_AdvancementSlayerManShadowFPK;			// Creature frames
		CCreatureFramePack		m_AdvancementSlayerWomanShadowFPK;			// Creature frames
		CCreatureFramePack		m_AdvancementVampireManShadowFPK;			// Player addon frames
		CCreatureFramePack		m_AdvancementVampireWomanShadowFPK;			// Player addon frames
		CCreatureFramePack		m_AdvancementOustersShadowFPK;			// Ousters addon frames

		//add by viva
		//                   Sprite
		//
		//------------------------------------------------------
		//------------------------------------------------------
		// SpritePack
		//------------------------------------------------------		
		CSpritePack				m_TileSPK;				// Tile
		CSpritePack				m_ImageObjectSPK;		

		CIndexSpritePack		m_CreatureSPK;			
		CIndexSpritePack		m_AddonSPK;		
		CIndexSpritePack		m_OustersSPK;		

		CIndexSpritePack		m_AdvancementSlayerManSPK;			
		CIndexSpritePack		m_AdvancementSlayerWomanSPK;			
		CIndexSpritePack		m_AdvancementVampireManSPK;		
		CIndexSpritePack		m_AdvancementVampireWomanSPK;		
		CIndexSpritePack		m_AdvancementOustersSPK;		
		
		//end
		
		
		CIndexSpritePack		m_ItemTileISPK;			
		CIndexSpritePack		m_ItemDropISPK;			//
		CSpritePack				m_ItemBrokenSPK;		
		CSpritePack				m_ItemRealSPK;			
		CAlphaSpritePalPack		m_EffectAlphaSPK;		
		CSpritePalPack			m_EffectScreenSPK;		// ScreenEffect
		MPalettePack			m_EffectAlphaPPK;		// AlphaEffect
		MPalettePack			m_EffectScreenPPK;		// ScreenEffect
		CSpritePack				m_EffectNormalSPK;		
		CShadowSpritePack		m_EffectShadowSPK;		

		// Effect resource container for safe access (refactored)
		EffectResourceContainer	m_EffectResources;

		CSpritePack				m_WeatherSPK;			
		CSpritePack				m_GuildSPK;				// GuildMark

		

		CSpritePack				m_EtcSPK;				
		CSpritePack				m_OustersFinSPK;				
		CSpritePack				m_AdvacementQuestEnding;				


		//------------------------------------------------------		
		// Shadow SpritePack
		//------------------------------------------------------
		CShadowSpritePack		m_AddonSSPK;			
		CShadowSpritePack		m_CreatureSSPK;
		CShadowSpritePack		m_OustersSSPK;
		CShadowSpritePack		m_ImageObjectSSPK;

		CShadowSpritePack		m_AdvancementSlayerManSSPK;			
		CShadowSpritePack		m_AdvancementSlayerWomanSSPK;			
		CShadowSpritePack		m_AdvancementVampireManSSPK;
		CShadowSpritePack		m_AdvancementVampireWomanSSPK;
		CShadowSpritePack		m_AdvancementOustersSSPK;
		//add by viva
//		CShadowSpritePack		m_InteractionObjectSSPK;

		//CShadowSpritePack		m_CreatureSSPK;

//		float					m_SectorToMinimapHeight;
//		CSpriteSurface*			m_pMinimapTexture;
//		int						m_MinimapTextureWidth;
//		int						m_MinimapTextureHeight;


		//------------------------------------------------------
		//
		//                  Sprite Index
		//
		//------------------------------------------------------
		CFileIndexTable			m_TileSPKI;
		CFileIndexTable			m_ImageObjectSPKI;
//		CFileIndexTable			m_EffectAlphaSPKI;
//		CFileIndexTable			m_EffectScreenSPKI;

		
		//------------------------------------------------------
		// 
		//                  Sprite File
		//
		//------------------------------------------------------
//		std::ifstream			m_TileSPKFile;
//		std::ifstream			m_ImageObjectSPKFile;
//		std::ifstream			m_EffectAlphaSPKFile;
//		std::ifstream			m_EffectScreenSPKFile;
//		std::ifstream			m_AddonSPKFile;
//		std::ifstream			m_AddonSSPKFile;
//		std::ifstream			m_CreatureSPKFile;
//		std::ifstream			m_CreatureSSPKFile;

		//------------------------------------------------------
		//
		//                   Filter
		//
		//------------------------------------------------------
		//CFilterPack				m_Filter;
		CFilter					m_ImageObjectFilter;

		//------------------------------------------------------
		//
		//               Texture PartManager
		//
		//------------------------------------------------------
		// Texture PartManager
//		CSpriteTexturePartManager*	m_pScreenEffectTextureManager;
//		CNormalSpriteTexturePartManager*	m_pImageObjectTextureManager;
//		CTexturePartManager*		m_pAlphaEffectTextureManager;
//		CShadowPartManager*			m_pImageObjectShadowManager;
//		CShadowPartManager*			m_pAddonShadowManager;
//		CShadowPartManager*			m_pCreatureShadowManager;
//		CShadowPartManager*			m_pOustersShadowManager;
//		CShadowPartManager*			m_pEffectShadowManager;


		//------------------------------------------------------
		
		//------------------------------------------------------
		CSpriteFilePositionArray*	m_pTileSFPArrayLargeZone;
		CSpriteFilePositionArray*	m_pImageObjectSFPArrayLargeZone;
		CSpriteFilePositionArray*	m_pTileSFPArraySmallZone;
		CSpriteFilePositionArray*	m_pImageObjectSFPArraySmallZone;


		//------------------------------------------------------
		//
		//                Other Data members
		//
		//------------------------------------------------------

		//------------------------------------------------------
		// Zone
		//------------------------------------------------------
		MZone*				m_pZone;				

		static int			m_MiddleX;				
		POINT				m_FirstSector;			
		POINT				m_PlusPoint;			
		POINT				m_FirstZonePixel;		

		//------------------------------------------------------
		
		//------------------------------------------------------
		POINT				m_SelectSector;		
		TYPE_OBJECTID		m_SelectCreatureID;
		TYPE_OBJECTID		m_SelectItemID;
//		TYPE_OBJECTID		m_SelectInteractionObjectID;
		TYPE_OBJECTID		m_SelectEffectID;
		//bool				m_bSelectModeAttack;

		WORD					m_SOMOutlineColor;
		CSpriteOutlineManager	m_SOM;	

		//------------------------------------------------------
		
		//------------------------------------------------------		
		POINT					m_FilterPosition;	
		BYTE					m_DarkBits;
		

		
		CFilterPack				m_LightFTP;
		CFilter					m_LightBufferFilter;
		CSpriteSurface*			m_pLightBufferTexture;
		int						m_nLight;


		//------------------------------------------------------
		
		//------------------------------------------------------		
		char				m_FadeValue;	
		char				m_FadeEnd;		
		char				m_FadeInc;		
		bool				m_bFade;		
		WORD				m_FadeColor;	
		WORD				m_delayFrame;	
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		CSpriteSurface*				m_pTileSurface;
		bool						m_bFirstTileDraw;
		int							m_TileSurfaceFirstZonePixelX,
									m_TileSurfaceFirstZonePixelY;
		int							m_TileSurfaceFirstSectorX,
									m_TileSurfaceFirstSectorY;

		//------------------------------------------------------
		// TileRenderer for unified tile rendering (Phase 4 integration)
		//------------------------------------------------------
		TileRenderer*				m_pTileRenderer;
		MZoneTileProvider			m_zoneTileProvider;

		//------------------------------------------------------
		
		//------------------------------------------------------
		IMAGEOBJECT_OUTPUT_MAP				m_mapImageObject;

		//------------------------------------------------------
		
		//------------------------------------------------------
		CREATURE_OUTPUT_MAP					m_mapCreature;
		bool								m_bTileSearchForCreature;

		//------------------------------------------------------
		
		//------------------------------------------------------
		POINT						m_pointChatString;
		MCreature*					m_pSelectedCreature;

		//------------------------------------------------------
		// Texture Surface
		//------------------------------------------------------
		// Effect
		//CTexturePack				m_EffectTPK;
		//CTexturePack				m_CreatureTPK;
		//CTexturePack				m_ShadowTPK;	// for test

#ifdef PLATFORM_WINDOWS
		CD3DTextureEffect			m_TextureEffect;
#endif
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		DRAWTEXT_PQ					m_pqDrawText;

		//------------------------------------------------------
		
		//------------------------------------------------------
		bool						m_bDrawItemNameList;
		DRAWITEMNAME_LIST			m_listDrawItemName;

		//------------------------------------------------------
		// Request mode
		//------------------------------------------------------
		bool						m_bDrawRequest;

		//------------------------------------------------------
		
		//------------------------------------------------------
//		CSpriteSurface*				m_p3DBoxSurface;
//		WORD						m_3DBoxCurrentPixel;

//		CSpriteSurface*				m_p3DBoxBlackSurface;
//		CSpriteSurface*				m_p3DBoxHPSurface;
		

		//------------------------------------------------------
		
		//------------------------------------------------------
		// interactionObject
//		WORD		m_ColorOutlineInteractionObject;

		// item
		COLORREF	m_ColorNameItem;
		COLORREF	m_ColorNameItemOption;
		WORD		m_ColorOutlineItem;

		
		COLORREF	m_ColorNameVampire;
		COLORREF	m_ColorNameSlayer;
		COLORREF	m_ColorNameNPC;

		COLORREF	m_ColorNameAlignment[5];	

		
		WORD		m_ColorOutlineNPC;
		WORD		m_ColorOutlineAttackPossible;
		WORD		m_ColorOutlineAttackImpossible;

		
		WORD		m_ColorHPBar;
		WORD		m_ColorHPBarBg;  

		
		WORD		m_ColorUIBoxBg;  

		
		WORD		m_ColorBlackHalf;


		//------------------------------------------------------
		
		//------------------------------------------------------
		int*		m_p2DLightPixelWidth;
		int*		m_p2DLightPixelHeight;

		//------------------------------------------------------
		
		//------------------------------------------------------
		INT_ORDERED_LIST	m_listLoadedCreatureSprite;
		INT_ORDERED_LIST	m_listLoadedMonsterSprite;
		
	public :
		
		static float				s_LightWidth;
		static float				s_LightHeight;
		
		
		enum GENERATE_OPTION{
			GENERATE_ALL = 0,
			GENERATE_EXCEPT_LEFT,	
			GENERATE_EXCEPT_RIGHT,  
			GENERATE_EXCEPT_SIDE,	
			
		};
		// 2004, 8, 18 sobeit add start

	private :
		
		//DrawCreatureExecptionProc*	m_pExceptionProc;
		void	InitMapingExceptionProc();
		void	RegistExceptionProcFunction( DWORD objectID, DrawCreatureExceptionProc proc );

		// DrawSelectedCharacter
		void	DrawSelectedVampireCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex );
		void	DrawSelectedSlayerCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawSelectedOustersCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );

		void	DrawSelectedAdvancementVampireCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex );
		void	DrawSelectedAdvancementSlayerCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawSelectedAdvancementOustersCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );

		// DrawEffect
		void	DrawInstallTurret( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int frameindex );
		void	DrawFadeOut( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawFastMove(POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawInvisible(POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawWeaponFadeOut(POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		
		void	DrawFadeOutForACSlayer( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawFastMoveForACSlayer(POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawInvisibleForACSlayer(POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawWeaponFadeOutForACSlayer(POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		
		// DrawDivineGuidance
		void	DrawDivineGuidanceSlayerCharacter( POINT *pPoint, MCreature* pCreature, int direction );
		void	DrawDivineGuidanceVampireCharacter( POINT *pPoint, MCreature* pCreature, int direction, int body );
		void	DrawDivineGuidanceOustersCharacter( POINT *pPoint, MCreature* pCreature, int direction );
		
		void	DrawDivineGuidanceAdvancementClassSlayerCharacter( POINT *pPoint, MCreature* pCreature, int direction );
		void	DrawDivineGuidanceAdvancementClassVampireCharacter( POINT *pPoint, MCreature* pCreature, int direction, int body );
		void	DrawDivineGuidanceAdvancementClassOustersCharacter( POINT *pPoint, MCreature* pCreature, int direction );
		
		// DrawNormalCharacter
		void	DrawSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int FrameIndex );		
		void	DrawVampireCharacter( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex );
		void	DrawOustersCharacter( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame );
		
		void	DrawShadowSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawShadowVampireCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int body, bool bBlendingShadow , bool bSlayerPet_ShowTurret );
		void	DrawShadowOustersCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame );

		// DrawAdvancementClassCharacter
		void	DrawAdvancementClassSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int FrameIndex );
		void	DrawAdvancementClassOustersCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawAdvancementClassVampireCharacter( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex );

		void	DrawShadowAdvancementClassSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame );
		void	DrawShadowAdvancementClassVampireCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int body, bool bBlendingShadow , bool bSlayerPet_ShowTurret );
		void	DrawShadowAdvancementClassOustersCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame );

		// 2004, 12, 11, sobeit add start
		void	DrawCentauroTurret( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame , int body);
		// 2004, 12, 11, sobeit add end
		
		// 2005, 1, 20, sobeit add start
		bool	DrawEvent();
		bool	ExcuteOustersFinEvent();
		bool	ExcuteAdvancementQuestEnding(void *pVoid);
		// 2005, 1, 20, sobeit add end
		
		std::map< DWORD, DrawCreatureExceptionProc >	m_ExceptionProcMap;
};

extern MTopView*	g_pTopView;

#endif
