//----------------------------------------------------------------------
// AddonDef.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__ADDONDEF_H__
#define	__ADDONDEF_H__


//----------------------------------------------------------------------

//----------------------------------------------------------------------
 
	

#ifdef __EXPO_CLIENT__
	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	enum ADDONID_MALE
	{
		ADDONID_HELM1_MALE,			
		ADDONID_COAT1_MALE,			
		ADDONID_TROUSER1_MALE,		
		ADDONID_GUN_SMG_MALE,			
		ADDONID_BLADE_MALE,		
		ADDONID_SHIELD1_MALE,		
		ADDONID_GUN_SG_MALE,			
		ADDONID_SWORD_MALE,		
		ADDONID_GUN_AR_MALE,			
		ADDONID_HAIR1_MALE,			
		ADDONID_HAIR2_MALE,			
		ADDONID_HAIR3_MALE,			
		ADDONID_HELM2_MALE,			
		ADDONID_COAT2_MALE,			
		ADDONID_TROUSER2_MALE,		
		ADDONID_CROSS_MALE,			
		ADDONID_GUN_TR_MALE,			
		ADDONID_MOTORCYCLE_MALE,	
		MAX_ADDONID_MALE
	};

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	enum ADDONID_FEMALE
	{
		ADDONID_HELM1_FEMALE,			
		ADDONID_COAT1_FEMALE,			
		ADDONID_TROUSER1_FEMALE,		
		ADDONID_GUN_SMG_FEMALE,			
		ADDONID_BLADE_FEMALE,		
		ADDONID_SHIELD_FEMALE,		
		ADDONID_GUN_SG_FEMALE,			
		ADDONID_SWORD_FEMALE,		
		ADDONID_GUN_AR_FEMALE,			
		ADDONID_HAIR1_FEMALE,			
		ADDONID_HAIR2_FEMALE,			
		ADDONID_HAIR3_FEMALE,			
		ADDONID_HELM2_FEMALE,			
		ADDONID_COAT2_FEMALE,			
		ADDONID_TROUSER2_FEMALE,		
		ADDONID_CROSS_FEMALE,			
		ADDONID_GUN_TR_FEMALE,			
		ADDONID_MOTORCYCLE_FEMALE,	
		MAX_ADDONID_FEMALE
	};

#else

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	enum ADDONID_MALE
	{
		ADDONID_HELM1_MALE = 0,
		ADDONID_COAT1_MALE,
		ADDONID_TROUSER1_MALE,
		ADDONID_GUN_SMG_MALE,		
		ADDONID_BLADE_MALE,			
		ADDONID_SHIELD1_MALE,
		ADDONID_SHIELD2_MALE,		
		ADDONID_SHIELD3_MALE,		
		ADDONID_GUN_SG_MALE,
		ADDONID_SWORD_MALE,	
		ADDONID_GUN_AR_MALE, 
		ADDONID_HAIR1_MALE,
		ADDONID_HAIR2_MALE,
		ADDONID_HAIR3_MALE,
		ADDONID_HELM2_MALE,
		ADDONID_COAT2_MALE,
		ADDONID_TROUSER2_MALE, 
		ADDONID_CROSS_MALE,
		ADDONID_GUN_SR_MALE,
		ADDONID_MOTORCYCLE_MALE,
		ADDONID_COAT0_MALE,
		ADDONID_TROUSER0_MALE,
		ADDONID_COAT3_MALE,
		ADDONID_TROUSER3_MALE,
		ADDONID_MACE_MALE,
	
		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		ADDONID_HELM1_FEMALE,
		ADDONID_COAT1_FEMALE,
		ADDONID_TROUSER1_FEMALE,
		ADDONID_GUN_SMG_FEMALE,		
		ADDONID_BLADE_FEMALE,			
		ADDONID_SHIELD1_FEMALE,
		ADDONID_SHIELD2_FEMALE,		
		ADDONID_SHIELD3_FEMALE,		
		ADDONID_GUN_SG_FEMALE,
		ADDONID_SWORD_FEMALE,	
		ADDONID_GUN_AR_FEMALE, 
		ADDONID_HAIR1_FEMALE,
		ADDONID_HAIR2_FEMALE,
		ADDONID_HAIR3_FEMALE,
		ADDONID_HELM2_FEMALE,
		ADDONID_COAT2_FEMALE,
		ADDONID_TROUSER2_FEMALE, 
		ADDONID_CROSS_FEMALE,
		ADDONID_GUN_SR_FEMALE,
		ADDONID_MOTORCYCLE_FEMALE,
		ADDONID_COAT0_FEMALE,
		ADDONID_TROUSER0_FEMALE,
		ADDONID_COAT3_FEMALE,
		ADDONID_TROUSER3_FEMALE,
		ADDONID_MACE_FEMALE,

		MAX_ADDONID_SLAYER
	};


#endif


#endif