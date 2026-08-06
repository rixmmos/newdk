//----------------------------------------------------------------------
// SoundDef.h
//----------------------------------------------------------------------

#ifndef	__SOUNDDEF_H__
#define	__SOUNDDEF_H__


//-------------------------------------------------------

//-------------------------------------------------------
enum SOUND_ID 
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SLAYER_ATTACK_BARE,
	SOUND_SLAYER_ATTACK_KICK,
	SOUND_SLAYER_ATTACK_MELEE,
	SOUND_SLAYER_ATTACK_AR,
	SOUND_SLAYER_ATTACK_SMG,
	SOUND_SLAYER_ATTACK_BOMB,
	SOUND_SLAYER_ATTACK_BOTTLE,
	SOUND_SLAYER_DIE_MALE,
	SOUND_SLAYER_DIE_FEMALE,
	SOUND_SLAYER_DAMAGE_MALE,
	SOUND_SLAYER_DAMAGE_FEMALE,
	SOUND_SLAYER_DRAINED_MALE,
	SOUND_SLAYER_DRAINED_FEMALE,
	SOUND_SLAYER_SWORD_MALE,
	SOUND_SLAYER_SWORD_FEMALE,
	SOUND_SLAYER_MAGIC_MALE,
	SOUND_SLAYER_MAGIC_FEMALE,

	//-------------------------------------------------------
	// Item
	//-------------------------------------------------------
	SOUND_ITEM_MOVE_CLOTH,
	SOUND_ITEM_MOVE_SWORD,
	SOUND_ITEM_MOVE_GUN,
	SOUND_ITEM_MOVE_SHIELD,
	SOUND_ITEM_MOVE_BRACELET,
	SOUND_ITEM_MOVE_SHOES,
	SOUND_ITEM_MOVE_GLOVE,
	SOUND_ITEM_MOVE_HELM,
	SOUND_ITEM_MOVE_BELT,
	SOUND_ITEM_MOVE_MAGAZINE,
	SOUND_ITEM_MOVE_SCROLL,
	SOUND_ITEM_MOVE_KEY,
	SOUND_ITEM_MOVE_BOMB,
	SOUND_ITEM_MOVE_BOTTLE,
	SOUND_ITEM_MOVE_C4,
	SOUND_ITEM_MOVE_SKULL,
	SOUND_ITEM_MOVE_MONEY,
	SOUND_ITEM_MOVE_POTION,
	SOUND_ITEM_USE_MPOTION,
	SOUND_ITEM_USE_HPOTION,
	SOUND_ITEM_USE_SCROLL,
	SOUND_ITEM_USE_C4,
	SOUND_ITEM_USE_MAGAZINE,
	SOUND_ITEM_NO_MAGAZINE,
	SOUND_ITEM_FLYING_BOMB,
	SOUND_ITEM_FLYING_BOTTLE,
	SOUND_DAMAGE_BOMB,	
	SOUND_DAMAGE_BOTTLE,
	SOUND_DAMAGE_GUN,
	SOUND_WORLD_BIKE_GO,
	SOUND_WORLD_BIKE_RUN,
	SOUND_WORLD_BIKE_STOP,
	SOUND_WORLD_BIKE_DAMAGE,
	SOUND_WORLD_BIKE_EXPLODE,

	//-------------------------------------------------------
	// Interface
	//-------------------------------------------------------
	SOUND_PDS_MENU,
	SOUND_PCS_1,
	SOUND_PCS_2,
	SOUND_PCS_3,
	SOUND_PCS_4,
	SOUND_PCS_5,
	SOUND_PCS_6,
	SOUND_PCS_7,
	SOUND_PCS_8,
	SOUND_PCS_9,
	SOUND_PCS_0,
	SOUND_PCS_STAR,
	SOUND_PCS_SHARP,
	SOUND_PDS_ETC,
	SOUND_SLAYER_BELT,
	SOUND_SLAYER_SKILL,
	SOUND_SLAYER_BUTTON,
	SOUND_SLAYER_MENU,
	SOUND_VAMPIRE_ICON,
	SOUND_VAMPIRE_BUTTON,


	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_VAMPIRE_CHANGE_MALE,
	SOUND_VAMPIRE_CHANGE_FEMALE,	
	SOUND_VAMPIRE_ATTACK,
	SOUND_VAMPIRE_DAMAGE_MALE,
	SOUND_VAMPIRE_DAMAGE_FEMALE,
	SOUND_VAMPIRE_DIE_MALE,
	SOUND_VAMPIRE_DIE_FEMALE,
	SOUND_VAMPIRE_DRAINED_MALE,
	SOUND_VAMPIRE_DRAINED_FEMALE,
	SOUND_VAMPIRE_DRAIN_MALE,
	SOUND_VAMPIRE_DRAIN_FEMALE,
	SOUND_VAMPIRE_MAGIC,
	
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_DB_ATTACK,
	SOUND_DB_DAMAGE,
	SOUND_DB_DIE,
	SOUND_DB_DRAINED,
	SOUND_DB_DRAIN,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_TD_ATTACK,
	SOUND_TD_DAMAGE,
	SOUND_TD_DIE,
	SOUND_TD_DRAINED,
	SOUND_TD_DRAIN,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_TS_ATTACK,
	SOUND_TS_DAMAGE,
	SOUND_TS_DIE,
	SOUND_TS_DRAINED,
	SOUND_TS_DRAIN,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_K_ATTACK,
	SOUND_K_DAMAGE,
	SOUND_K_DIE,
	SOUND_K_DRAINED,
	SOUND_K_DRAIN,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_S_ATTACK,
	SOUND_S_DAMAGE,
	SOUND_S_DIE,
	SOUND_S_DRAINED,
	SOUND_S_DRAIN,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_C_ATTACK,
	SOUND_C_DAMAGE,
	SOUND_C_DIE,
	SOUND_C_DRAINED,
	SOUND_C_DRAIN,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SW_ATTACK,
	SOUND_SW_DAMAGE,
	SOUND_SW_DIE,
	SOUND_SW_DRAINED,
	SOUND_SW_DRAIN,
	SOUND_SW_MAGIC,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_DS_ATTACK,
	SOUND_DS_DAMAGE,
	SOUND_DS_DIE,
	SOUND_DS_DRAINED,
	SOUND_DS_DRAIN,
	SOUND_DS_MAGIC,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_BW_ATTACK,
	SOUND_BW_DAMAGE,
	SOUND_BW_DIE,
	SOUND_BW_DRAINED,
	SOUND_BW_DRAIN,
	SOUND_BW_MAGIC,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_GO_MOVE,
	SOUND_GO_ATTACK,
	SOUND_GO_DAMAGE,
	SOUND_GO_DIE,
	SOUND_GO_DRAINED,
	SOUND_GO_DRAIN,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_RE_ATTACK,
	SOUND_RE_DAMAGE,
	SOUND_RE_DIE,
	SOUND_RE_DRAINED,
	SOUND_RE_DRAIN,
	SOUND_RE_MAGIC,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_AL_ATTACK,
	SOUND_AL_DAMAGE,
	SOUND_AL_DIE,
	SOUND_AL_DRAINED,
	SOUND_AL_DRAIN,
	SOUND_AL_MAGIC,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_CG_ATTACK,
	SOUND_CG_DAMAGE,
	SOUND_CG_DIE,
	SOUND_CG_DRAINED,
	SOUND_CG_DRAIN,	

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_WORLD_DOG_BARK,
	SOUND_WORLD_DOG_GRRR,
	SOUND_WORLD_DOG_KICKED,
	SOUND_WORLD_WOLF_CRY,
	SOUND_WORLD_CROW,
	SOUND_WORLD_BIRD,
	SOUND_WORLD_WEATHER_RAIN_1,
	SOUND_WORLD_WEATHER_RAIN_2,
	SOUND_WORLD_WEATHER_THUNDER_1,
	SOUND_WORLD_WEATHER_THUNDER_2,
	SOUND_WORLD_WEATHER_WIND,
	SOUND_WORLD_DOOR_WOOD,
	SOUND_WORLD_DOOR_IRON,
	SOUND_WORLD_WATER_DROP,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SLAYER_SWORD_DI,
	SOUND_SLAYER_SWORD_TS,
	SOUND_SLAYER_SWORD_SS,
	SOUND_SLAYER_SWORD_RS,
	SOUND_SLAYER_SWORD_THS,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SLAYER_BLADE_SB,
	SOUND_SLAYER_BLADE_TB,
	SOUND_SLAYER_BLADE_SS,
	SOUND_SLAYER_BLADE_TRB,
	SOUND_SLAYER_BLADE_WS,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SLAYER_ENCHANT_B1,	
	SOUND_SLAYER_ENCHANT_B2,	
	SOUND_SLAYER_ENCHANT_CHW1,	
	SOUND_SLAYER_ENCHANT_CHW2,	
	SOUND_SLAYER_ENCHANT_CL,	// continual light
	SOUND_SLAYER_ENCHANT_DH,	// detect hidden
	SOUND_SLAYER_ENCHANT_L,		// light
	SOUND_SLAYER_ENCHANT_S1,	
	SOUND_SLAYER_ENCHANT_S2,	

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SLAYER_HEALING_CCW,	// cure critical wounds
	SOUND_SLAYER_HEALING_CLW,	// cure light wounds
	SOUND_SLAYER_HEALING_CP,	// cure poison
	SOUND_SLAYER_HEALING_CPA,	// cure paralysis
	SOUND_SLAYER_HEALING_PFC,	// protection from curse
	SOUND_SLAYER_HEALING_PFP,	// protection from poison
	SOUND_SLAYER_HEALING_PFPA,	// protection from paralysis
	SOUND_SLAYER_HEALING_R1,	
	SOUND_SLAYER_HEALING_R2,	
	SOUND_SLAYER_HEALING_RC,	// remove curse

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_VAMPIRE_ACID_ABOLT1,		
	SOUND_VAMPIRE_ACID_ABOLT2,		
	SOUND_VAMPIRE_MISC_BD,			// blood drain
	SOUND_VAMPIRE_MISC_H,			// hide
	SOUND_VAMPIRE_POISON_GP,		// green poison
	SOUND_VAMPIRE_POISON_PW,		// purple wall
	SOUND_VAMPIRE_POISON_YP,		// yellow poison
	SOUND_VAMPIRE_ACID_AT,			// acid touch
	SOUND_VAMPIRE_MISC_D,			// darkness
	SOUND_VAMPIRE_POISON_PH,		// poisonous hand

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_HB_ATTACK,
	SOUND_HB_DAMAGE,
	SOUND_HB_DIE,
	SOUND_HB_DRAINED,
	SOUND_HB_DRAIN,
	SOUND_HB_MAGIC,


	

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_BAT_DAMAGE,
	SOUND_BAT_DIE,
	SOUND_BAT_WING,

	//-------------------------------------------------------
	// ITEM
	//-------------------------------------------------------
	SOUND_ITEM_MOVE_HEAD,
	SOUND_ITEM_MOVE_NECKLACE,
	SOUND_ITEM_MOVE_RING,

	//-------------------------------------------------------
	// slayer
	//-------------------------------------------------------
	SOUND_SLAYER_ATTACK_SG,
	SOUND_SLAYER_ATTACK_SILENCER,
	SOUND_SLAYER_ATTACK_SR,
	SOUND_SLAYER_CHARGE,

	SOUND_SLAYER_ENCHANT_AURABALL1,
	SOUND_SLAYER_ENCHANT_AURABALL2,
	SOUND_SLAYER_ENCHANT_AURARING1,
	SOUND_SLAYER_ENCHANT_AURARING2,
	SOUND_SLAYER_ENCHANT_AURASHIELD1,
	SOUND_SLAYER_ENCHANT_AURASHIELD2,
	SOUND_SLAYER_ENCHANT_BLESSGROUND,

	SOUND_SLAYER_HEALING_CSW,
	SOUND_SLAYER_HEALING_REMOVECURSE,

	//-------------------------------------------------------
	// Vampire
	//-------------------------------------------------------
	SOUND_VAMPIRE_ACIDBALL1,
	SOUND_VAMPIRE_ACIDBALL2,
	SOUND_VAMPIRE_ACIDBURN,
	SOUND_VAMPIRE_DOOM,
	SOUND_VAMPIRE_GREENSTALKER1,
	SOUND_VAMPIRE_GREENSTALKER2,

	SOUND_VAMPIRE_PARALYSIS,
	SOUND_VAMPIRE_POISON_DB,
	SOUND_VAMPIRE_TRANSFORM,
	
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_WOLF_ATTACK,
	SOUND_WOLF_DAMAGE,
	SOUND_WOLF_DIE,
	SOUND_WOLF_DRAINED,
	SOUND_WOLF_EAT,
	SOUND_WOLF_GROWL,
	SOUND_WOLF_HOWL,

	//-------------------------------------------------------
	// World
	//-------------------------------------------------------
	SOUND_WORLD_COCK,
	SOUND_WORLD_CRICKET,
	SOUND_WORLD_FROG,
	SOUND_WORLD_GRASS,
	SOUND_WORLD_OWL,
	SOUND_WORLD_PROPELLER,
	SOUND_WORLD_RATS,
	SOUND_WORLD_RIVER,
	SOUND_WORLD_SEWER,
	SOUND_WORLD_TREE,
	SOUND_WORLD_WIND,	
	
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_BLOOD_SWORD,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_ES_ATTACK,
	SOUND_ES_DAMAGE,
	SOUND_ES_DIE,
	SOUND_ES_DRAIN,
	SOUND_ES_DRAINED,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_MD_ATTACK,
	SOUND_MD_DAMAGE,
	SOUND_MD_DIE,
	SOUND_MD_DRAIN,
	SOUND_MD_DRAINED,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_MT_ATTACK,
	SOUND_MT_DAMAGE,
	SOUND_MT_DIE,
	SOUND_MT_DRAIN,
	SOUND_MT_DRAINED,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_WD_ATTACK,
	SOUND_WD_DAMAGE,
	SOUND_WD_DIE,
	SOUND_WD_DRAIN,
	SOUND_WD_DRAINED,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_EVENT_FANFARE,


	

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_BF_ATTACK,
	SOUND_BF_DAMAGE,
	SOUND_BF_DIE,
	SOUND_BF_DRAIN,
	SOUND_BF_DRAINED,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_VD_ATTACK,
	SOUND_VD_DAMAGE,
	SOUND_VD_DIE,
	SOUND_VD_DRAIN,
	SOUND_VD_DRAINED,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SLAYER_BLADE_TS,						// tornado sever
	SOUND_SLAYER_ENCHANT_DETECTINVISIBILITY,
	SOUND_SLAYER_ENCHANT_IDENTIFY,
	SOUND_SLAYER_ENCHANT_PFA,						// protection from acid
	SOUND_SLAYER_HEAL_SACRIFICE,
	SOUND_SLAYER_SWORD_LH,						// lightning hands
	SOUND_SLAYER_SWORD_SC,						// snake combo
	SOUND_SLAYER_SWORD_SW,						// sword wave

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_VAMPIRE_ACIDSWAMP,		


	

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_CK_ATTACK,
	SOUND_CK_DAMAGE,
	SOUND_CK_DIE,
	SOUND_CK_DRAIN,
	SOUND_CK_DRAINED,
	SOUND_CK_MAGIC,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_CS_ATTACK,
	SOUND_CS_DAMAGE,
	SOUND_CS_DIE,
	SOUND_CS_DRAIN,
	SOUND_CS_DRAINED,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_DSC_ATTACK,
	SOUND_DSC_DAMAGE,
	SOUND_DSC_DIE,
	SOUND_DSC_DRAIN,
	SOUND_DSC_DRAINED,
	SOUND_DSC_MAGIC,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_IT_ATTACK,
	SOUND_IT_DAMAGE,
	SOUND_IT_DIE,
	SOUND_IT_DRAIN,
	SOUND_IT_DRAINED,

	//-------------------------------------------------------
	// item
	//-------------------------------------------------------
	SOUND_ITEM_MOVE_JEWEL,

	//-------------------------------------------------------
	// world
	//-------------------------------------------------------
	SOUND_WORLD_MAP,
	SOUND_WORLD_WALKIETALKIE,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_ITEM_BOOM1,
	SOUND_ITEM_BOOM2,
	SOUND_ITEM_BOOM3,
	SOUND_ITEM_BOOM4,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_SLAYER_BLADE_BERSERKER,
	SOUND_SLAYER_BLADE_EARTHQUAKE,
	SOUND_SLAYER_ENCHANT_ENCHANT,
	SOUND_SLAYER_ENCHANT_VISIBLEA,
	SOUND_SLAYER_ENCHANT_VISIBLEB,
	SOUND_SLAYER_HEAL_ACTIVATION,
	SOUND_SLAYER_HEAL_CUREALL,
	SOUND_SLAYER_SWORD_EXPANSION,
	SOUND_SLAYER_SWORD_THUNDERBOLTA,
	SOUND_SLAYER_SWORD_THUNDERBOLTB,
	SOUND_SLAYER_SWORD_WINDDIVIDER,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_VAMPIRE_BLOODYBALL,
	SOUND_VAMPIRE_BLOODYKNIFE,
	SOUND_VAMPIRE_BLOODYWALL,
	SOUND_VAMPIRE_DEATH,
	SOUND_VAMPIRE_HALLUCINATION,
	SOUND_VAMPIRE_SEDUCTION,
	SOUND_VAMPIRE_SUMMONCASKET,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_XMAS_GIFTBOX,
	SOUND_XMAS_STAR,

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	SOUND_VOL_MAGIC,
	SOUND_VOL_DIE,
	SOUND_VOL_DAMAGE,
	SOUND_VOL_ATTACK,

	SOUND_ASH_DIE,
	SOUND_ASH_DAMAGE,
	SOUND_ASH_ATTACK,
	
	SOUND_DIM_DIE,
	SOUND_DIM_DAMAGE,
	SOUND_DIM_ATTACK,
	SOUND_DIM_MAGIC,

	//------------------------------------------------------------
	// 2003.5.16
	//------------------------------------------------------------
	SOUND_DRAGON_FIRE_CRACKER,
	SOUND_AMPUTATE,
	SOUND_CONCEALMENT,
	SOUND_DENIAL_MAGIC_DAMAGED,
	SOUND_DENIAL_MAGIC_START,
	SOUND_REQUITAL,

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	SOUND_OUSTERS_ARMSBAND,
	SOUND_OUSTERS_BOOTS,
	SOUND_OUSTERS_CHAKRAM,
	SOUND_OUSTERS_CIRCLET,
	SOUND_OUSTERS_COAT,
	SOUND_OUSTERS_ELEMENTAL_STONE,
	SOUND_OUSTERS_INTERFACE,
	SOUND_OUSTERS_PENDANT,
	SOUND_OUSTERS_RING,
	SOUND_OUSTERS_SKILL_INFO,
	SOUND_OUSTERS_WRISTLET,

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	SOUND_OUSTERS_ATTACK_1, 
	SOUND_OUSTERS_ATTACK_2, 
	SOUND_OUSTERS_MAGIC_1,  // 
	SOUND_OUSTERS_MAGIC_2,
	SOUND_OUSTERS_DAMAGED,
	SOUND_OUSTERS_FAST_MOVE_1,
	SOUND_OUSTERS_FAST_MOVE_2,
	SOUND_OUSTERS_DIE,
	SOUND_OUSTERS_ABSORB_SOUL,
	SOUND_OUSTERS_DRAIN,

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	SOUND_OUSTERS_PROMINENCE,
	SOUND_OUSTERS_BACK_STAB,
	SOUND_OUSTERS_BLAZE_BOLT,
	SOUND_OUSTERS_BLUNTING,
	SOUND_OUSTERS_CROSS_GUARD,
	SOUND_OUSTERS_EVADE,
	SOUND_OUSTERS_FLOURISH,
	SOUND_OUSTERS_GAMMA_CHOP,
	SOUND_OUSTERS_LIBERTY,
	SOUND_OUSTERS_SHARP_ROUND,
	
	SOUND_OUSTERS_BLOCK_HEAD,
	SOUND_OUSTERS_GNOME_WHISPER,
	SOUND_OUSTERS_EARTH_TEETH,
	SOUND_OUSTERS_SAND_OF_SOUL_STONE,
	SOUND_OUSTERS_ICE_OF_SOUL_STONE,
	SOUND_OUSTERS_FIRE_OF_SOUL_STONE,
	SOUND_OUSTERS_REFUSAL_ETHER,
	SOUND_OUSTERS_TENDRIL,
	SOUND_OUSTERS_HANDS_OF_FIRE,
	SOUND_OUSTERS_ICE_FIELD,
	SOUND_OUSTERS_RING_OF_FLARE,
	SOUND_OUSTERS_PROMINENCE_2,
	SOUND_OUSTERS_HANDS_OF_NIZIE,
	SOUND_OUSTERS_STONE_AUGER,
	SOUND_OUSTERS_NYMPH_RECOVERY,
	SOUND_OUSTERS_WATER_BARRIER,
	
	SOUND_OUSTERS_PUPA,
	SOUND_OUSTERS_MEI,
	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	SOUND_SLAYER_BLITZ_SLIDING,
	SOUND_SLAYER_BLAZE_WALK,
	SOUND_SLAYER_JABBING_VEIN,
	SOUND_SLAYER_GREAT_HEAL,
	SOUND_SLAYER_DIVINE_GUIDANCE,
	SOUND_VAMPIRE_BLOODY_ZENITH,

	
	SOUND_REGEN_TOWER_REPEAT,
	SOUND_REGEN_TOWER_START,
	SOUND_REGEN_TOWER_FLASH,
	SOUND_REGEN_TOWER_OWNER_CHANGED,
	SOUND_SWEEPER_START,
	SOUND_SWEEPER_REPEAT,
	SOUND_SWEEPER_END,

	
	SOUND_DESTRUCTION_SPEAR,
	SOUND_DUCKING_WALLOP,
	SOUND_FATAL_SNICK,
	SOUND_FROZEN_ARMOR,
	SOUND_GROUND_BLESS,
	SOUND_HELLFIRE,
	SOUND_ICE_LANCE,
	SOUND_MAGNUM_SPEAR_1,
	SOUND_MAGNUM_SPEAR_2,
	SOUND_MAGNUM_SPEAR_3,
	SOUND_REACTIVE_ARMOR,
	SOUND_SAND_CROSS,
	SOUND_SHIFT_BREAK,
	SOUND_SOUL_REBIRTH,
	SOUND_TELEPORT,

	//2004,04,27 sobeit 2nd pet sound add start
	SOUND_WORLD_CENTAURO_ATTACK,
	SOUND_WORLD_CENTAURO_MOVE,
	SOUND_WORLD_CENTAURO_PICKUP,
	SOUND_WORLD_STIRGE_ATTACK,
	SOUND_WORLD_STIRGE_MOVE,
	SOUND_WORLD_PIXIE_ABSORB,
	SOUND_WORLD_PIXIE_MOVE,
	//2004,04,27 sobeit 2nd pet sound add end

	
	SOUND_BLIND_SUMMON,		
	SOUND_BLIND_DESTORY,	
	SOUND_PIPE_ROUTING,		
	SOUND_PIPE_DROP,		
	SOUND_BLINDNESS,		
	SOUND_TRAP_A,			
	SOUND_TRAP_B,			
	SOUND_DROP_A,			
	SOUND_SIDE_SUMMON,		
	SOUND_SIDE_DESTORY,		
	SOUND_GLACIER_START,	
	SOUND_GLACIER_END,		
	SOUND_ICEAUGER,			
	SOUND_ICEHAIL,			
	SOUND_ICEWAVE,			
	SOUND_GDR_GHOST1,		
	SOUND_GDR_GHOST2,		
	SOUND_GDR_GHOST3,		
	SOUND_GDR_GHOST4,		
	
	
	
	SOUND_SLAYER_TURRET_FIRE,
	SOUND_SLAYER_TURRET_FIRE_START,
	SOUND_SLAYER_TURRET_FIRE_END,
	SOUND_SLAYER_SWORDOFTHOR,
	SOUND_SLAYER_WITHSUNTIDE,
	SOUND_SLAYER_SWEEPVICE_HIT,
	SOUND_SLAYER_SWEEPVICE_RESIZE,
	SOUND_VAMPIRE_SUMMONGOREGRAND,
	
	
	
	SOUND_GREAT_RUFFIAN_AXE,
	// 2004, 11, 8, sobeit add end

	
	SOUND_SLAYER_PLASMA_ROCKET_LAUNCHER,
	SOUND_SLAYER_BOMBING_STAR,
	SOUND_SLAYER_INTIMATE_GRAIL,
	SOUND_OUSTERS_FURY_OF_GNOME,
	SOUND_OUSTERS_SUMMON_MIGA,
	
	
	
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_FLIEGER_BOMB,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_GEFREITER_ATTACK,
	SOUND_GEFREITER_DAMAGE,
	SOUND_GEFREITER_DEAD,
	SOUND_GEFREITER_DRAINED,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_HAUPTMANN_DAMAGE,
	SOUND_HAUPTMANN_DEAD,
	SOUND_HAUPTMANN_DRAINED,
	SOUND_HAUPTMANN_MAGIC,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_LUNGA_TESTA_ATTACK,
	SOUND_LUNGA_TESTA_DAMAGE,
	SOUND_LUNGA_TESTA_DEAD,
	SOUND_LUNGA_TESTA_DRAINED,
	SOUND_LUNGA_TESTA_VAMPIRE_DRAIN,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_OBERSCHUTZE_SR_ATTACK,
	SOUND_OBERSCHUTZE_SR_DAMAGE,
	SOUND_OBERSCHUTZE_SR_DEAD,
	SOUND_OBERSCHUTZE_SR_DRAINED,
	SOUND_OBERSCHUTZE_SR_MELEE,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_ROI_CADAVRU_ATTACK,
	SOUND_ROI_CADAVRU_DAMAGE,
	SOUND_ROI_CADAVRU_DEAD,
	SOUND_ROI_CADAVRU_DRAINED,
	SOUND_ROI_CADAVRU_VAMPIRE_DRAIN,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_SIAMESE_ATTACK,
	SOUND_SIAMESE_DAMAGE,
	SOUND_SIAMESE_DEAD,
	SOUND_SIAMESE_DRAINED,
	SOUND_SIAMESE_VAMPIRE_DRAIN,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_STURMMANN_SMG_ATTACK,
	SOUND_STURMMANN_SMG_DAMAGE,
	SOUND_STURMMANN_SMG_DEAD,
	SOUND_STURMMANN_SMG_DRAINED,
	//---------------------------------------------
	
	//---------------------------------------------
	SOUND_TUGLEG_ATTACK,
	SOUND_TUGLEG_DAMAGE,
	SOUND_TUGLEG_DEAD,
	SOUND_TUGLEG_DRAINED,
	SOUND_TUGLEG_MOVE,
	SOUND_TUGLEG_VAMPIRE_DRAIN,
	
	MAX_SOUND
};

#endif