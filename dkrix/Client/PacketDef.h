//----------------------------------------------------------------------
// PacketDef.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
#ifndef	__PACKETDEF_H__
#define	__PACKETDEF_H__

//-----------------------------------------------------------------------------
// Login
//-----------------------------------------------------------------------------
#include "CLVersionCheck.h"
#include "CLLogout.h"
#include "Packet/Cpackets/CLLogin.h"
#include "CLRegisterPlayer.h"
#include "CLGetPCList.h"
#include "Packet/Cpackets/CLCreatePC.h"
#include "CLDeletePC.h"
#include "CLSelectPC.h"
#include "CLQueryPlayerID.h"

//-----------------------------------------------------------------------------
// core
//-----------------------------------------------------------------------------
#include "Packet/PacketFactoryManager.h"
#include "Packet/PacketValidator.h"
#include "Packet/Properties.h"
#include "Packet/ClientPlayer.h"
#include "Packet/Socket.h"

//-----------------------------------------------------------------------------
// connect
//-----------------------------------------------------------------------------
#include "CGConnect.h"
#include "CGReady.h"
#include "Packet/Cpackets/CGMove.h"
#include "CGLogout.h"

//-----------------------------------------------------------------------------
// Phone / Whisper
//-----------------------------------------------------------------------------
#include "CGDialUp.h"
#include "CGPhoneDisconnect.h"
#include "CGPhoneSay.h"
#include "CGWhisper.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGSay.h"
#include "CGGlobalChat.h"


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "CGAttack.h"
#include "CGCastingSkill.h"
#include "Packet/Cpackets/CGSkillToInventory.h"
#include "Packet/Cpackets/CGSkillToSelf.h"
#include "CGSkillToTile.h"
#include "Packet/Cpackets/CGSkillToObject.h"
#include "Packet/Cpackets/CGUnburrow.h"
#include "Packet/Cpackets/CGBloodDrain.h"

#include "CGThrowBomb.h"
#include "CGUntransform.h"
#include "CGVisible.h"
#include "Packet/Cpackets/CGAbsorbSoul.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "CGLearnSkill.h"
#include "Packet/Cpackets/CGUseBonusPoint.h"		// for vampire lev up
#include "CGSetSlayerHotKey.h"
#include "CGSetVampireHotKey.h"


//-----------------------------------------------------------------------------
// Item 
//-----------------------------------------------------------------------------
#include "CGAddGearToMouse.h"
#include "Packet/Cpackets/CGAddInventoryToMouse.h"
#include "CGAddMouseToGear.h"
#include "Packet/Cpackets/CGAddMouseToInventory.h"
#include "CGAddMouseToQuickSlot.h"
#include "CGAddMouseToZone.h"
#include "CGAddQuickSlotToMouse.h"
#include "CGAddZoneToInventory.h"
#include "CGAddZoneToMouse.h"
#include "CGPickupMoney.h"
#include "CGDropMoney.h"
#include "CGRequestRepair.h"
#include "Packet/Cpackets/CGMakeItem.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGDissectionCorpse.h"


#include "CGReloadFromInventory.h"
#include "CGReloadFromQuickSlot.h"


#include "Packet/Cpackets/CGGetOffMotorCycle.h"
#include "Packet/Cpackets/CGRideMotorCycle.h"


#include "Packet/Cpackets/CGThrowItem.h"

// potion
#include "Packet/Cpackets/CGUsePotionFromInventory.h"
#include "CGUsePotionFromQuickSlot.h"

#include "Packet/Cpackets/CGUseItemFromInventory.h"

//-----------------------------------------------------------------------------
// NPC
//-----------------------------------------------------------------------------
#include "CGNPCTalk.h"
#include "CGNPCAskAnswer.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "CGShopRequestList.h"
#include "CGShopRequestBuy.h"
#include "CGShopRequestSell.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGStashRequestBuy.h"
#include "Packet/Cpackets/CGStashList.h"
#include "Packet/Cpackets/CGMouseToStash.h"
#include "Packet/Cpackets/CGStashToMouse.h"
#include "Packet/Cpackets/CGStashDeposit.h"
#include "Packet/Cpackets/CGStashWithdraw.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "CGTradePrepare.h"
#include "CGTradeAddItem.h"
#include "CGTradeRemoveItem.h"
#include "CGTradeMoney.h"
#include "CGTradeFinish.h"





#endif