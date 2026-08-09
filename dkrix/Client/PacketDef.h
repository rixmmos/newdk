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
#include "Packet/Cpackets/CLDeletePC.h"
#include "Packet/Cpackets/CLSelectPC.h"
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
#include "Packet/Cpackets/CGDialUp.h"
#include "Packet/Cpackets/CGPhoneDisconnect.h"
#include "CGPhoneSay.h"
#include "CGWhisper.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGSay.h"
#include "CGGlobalChat.h"


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "CGAttack.h"
#include "Packet/Cpackets/CGCastingSkill.h"
#include "Packet/Cpackets/CGSkillToInventory.h"
#include "Packet/Cpackets/CGSkillToSelf.h"
#include "Packet/Cpackets/CGSkillToTile.h"
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
#include "Packet/Cpackets/CGAddGearToMouse.h"
#include "Packet/Cpackets/CGAddInventoryToMouse.h"
#include "Packet/Cpackets/CGAddMouseToGear.h"
#include "Packet/Cpackets/CGAddMouseToInventory.h"
#include "Packet/Cpackets/CGAddMouseToQuickSlot.h"
#include "CGAddMouseToZone.h"
#include "Packet/Cpackets/CGAddQuickSlotToMouse.h"
#include "CGAddZoneToInventory.h"
#include "CGAddZoneToMouse.h"
#include "Packet/Cpackets/CGPickupMoney.h"
#include "CGDropMoney.h"
#include "Packet/Cpackets/CGRequestRepair.h"
#include "Packet/Cpackets/CGMakeItem.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGDissectionCorpse.h"


#include "Packet/Cpackets/CGReloadFromInventory.h"
#include "Packet/Cpackets/CGReloadFromQuickSlot.h"


#include "Packet/Cpackets/CGGetOffMotorCycle.h"
#include "Packet/Cpackets/CGRideMotorCycle.h"


#include "Packet/Cpackets/CGThrowItem.h"

// potion
#include "Packet/Cpackets/CGUsePotionFromInventory.h"
#include "Packet/Cpackets/CGUsePotionFromQuickSlot.h"

#include "Packet/Cpackets/CGUseItemFromInventory.h"

//-----------------------------------------------------------------------------
// NPC
//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGNPCTalk.h"
#include "CGNPCAskAnswer.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "CGShopRequestList.h"
#include "Packet/Cpackets/CGShopRequestBuy.h"
#include "Packet/Cpackets/CGShopRequestSell.h"

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