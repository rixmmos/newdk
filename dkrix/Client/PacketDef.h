//----------------------------------------------------------------------
// PacketDef.h
//----------------------------------------------------------------------
// Client에서 사용하는 Packet에 대한 정의
//----------------------------------------------------------------------
#ifndef	__PACKETDEF_H__
#define	__PACKETDEF_H__

//-----------------------------------------------------------------------------
// Login
//-----------------------------------------------------------------------------
#include "../../shared/Packets/CLVersionCheck.h"
#include "../../shared/Packets/CLLogout.h"
#include "../../shared/Packets/CLLogin.h"
#include "../../shared/Packets/CLRegisterPlayer.h"
#include "../../shared/Packets/CLGetPCList.h"
#include "../../shared/Packets/CLCreatePC.h"
#include "../../shared/Packets/CLDeletePC.h"
#include "../../shared/Packets/CLSelectPC.h"
#include "../../shared/Packets/CLQueryPlayerID.h"

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
#include "../../shared/Packets/CGConnect.h"
#include "../../shared/Packets/CGReady.h"
#include "Packet/Cpackets/CGMove.h"
#include "../../shared/Packets/CGLogout.h"

//-----------------------------------------------------------------------------
// Phone / Whisper
//-----------------------------------------------------------------------------
#include "../../shared/Packets/CGDialUp.h"
#include "../../shared/Packets/CGPhoneDisconnect.h"
#include "../../shared/Packets/CGPhoneSay.h"
#include "Packet/Cpackets/CGWhisper.h"

//-----------------------------------------------------------------------------
// 채팅
//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGSay.h"
#include "../../shared/Packets/CGGlobalChat.h"


//-----------------------------------------------------------------------------
// 기술 사용
//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGAttack.h"
#include "Packet/Cpackets/CGCastingSkill.h"
#include "../../shared/Packets/CGSkillToInventory.h"
#include "../../shared/Packets/CGSkillToSelf.h"
#include "../../shared/Packets/CGSkillToTile.h"
#include "../../shared/Packets/CGSkillToObject.h"
#include "../../shared/Packets/CGUnburrow.h"
#include "../../shared/Packets/CGBloodDrain.h"
// 2001.4.23에 추가
#include "Packet/Cpackets/CGThrowBomb.h"
#include "../../shared/Packets/CGUntransform.h"
#include "../../shared/Packets/CGVisible.h"
#include "Packet/Cpackets/CGAbsorbSoul.h"

//-----------------------------------------------------------------------------
// skill 배움 / skill 단축키
//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGLearnSkill.h"
#include "../../shared/Packets/CGUseBonusPoint.h"		// for vampire lev up
#include "../../shared/Packets/CGSetSlayerHotKey.h"
#include "../../shared/Packets/CGSetVampireHotKey.h"


//-----------------------------------------------------------------------------
// Item 
//-----------------------------------------------------------------------------
#include "../../shared/Packets/CGAddGearToMouse.h"
#include "../../shared/Packets/CGAddInventoryToMouse.h"
#include "../../shared/Packets/CGAddMouseToGear.h"
#include "../../shared/Packets/CGAddMouseToInventory.h"
#include "../../shared/Packets/CGAddMouseToQuickSlot.h"
#include "../../shared/Packets/CGAddMouseToZone.h"
#include "../../shared/Packets/CGAddQuickSlotToMouse.h"
#include "../../shared/Packets/CGAddZoneToInventory.h"
#include "../../shared/Packets/CGAddZoneToMouse.h"
#include "../../shared/Packets/CGPickupMoney.h"
#include "Packet/Cpackets/CGDropMoney.h"
#include "Packet/Cpackets/CGRequestRepair.h"
#include "Packet/Cpackets/CGMakeItem.h"

//-----------------------------------------------------------------------------
// 시체 click
//-----------------------------------------------------------------------------
#include "../../shared/Packets/CGDissectionCorpse.h"

// 탄창
#include "../../shared/Packets/CGReloadFromInventory.h"
#include "../../shared/Packets/CGReloadFromQuickSlot.h"

// 오토바이
#include "../../shared/Packets/CGGetOffMotorCycle.h"
#include "Packet/Cpackets/CGRideMotorCycle.h"

// 던지기
#include "Packet/Cpackets/CGThrowItem.h"

// potion
#include "../../shared/Packets/CGUsePotionFromInventory.h"
#include "../../shared/Packets/CGUsePotionFromQuickSlot.h"

#include "../../shared/Packets/CGUseItemFromInventory.h"

//-----------------------------------------------------------------------------
// NPC
//-----------------------------------------------------------------------------
#include "Packet/Cpackets/CGNPCTalk.h"
#include "../../shared/Packets/CGNPCAskAnswer.h"

//-----------------------------------------------------------------------------
// 상점
//-----------------------------------------------------------------------------
#include "../../shared/Packets/CGShopRequestList.h"
#include "../../shared/Packets/CGShopRequestBuy.h"
#include "../../shared/Packets/CGShopRequestSell.h"

//-----------------------------------------------------------------------------
// 보관
//-----------------------------------------------------------------------------
#include "../../shared/Packets/CGStashRequestBuy.h"
#include "../../shared/Packets/CGStashList.h"
#include "../../shared/Packets/CGMouseToStash.h"
#include "../../shared/Packets/CGStashToMouse.h"
#include "../../shared/Packets/CGStashDeposit.h"
#include "../../shared/Packets/CGStashWithdraw.h"

//-----------------------------------------------------------------------------
// 교환
//-----------------------------------------------------------------------------
#include "../../shared/Packets/CGTradePrepare.h"
#include "../../shared/Packets/CGTradeAddItem.h"
#include "../../shared/Packets/CGTradeRemoveItem.h"
#include "../../shared/Packets/CGTradeMoney.h"
#include "../../shared/Packets/CGTradeFinish.h"





#endif
