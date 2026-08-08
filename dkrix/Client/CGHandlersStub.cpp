/*-----------------------------------------------------------------------------

	CGHandlersStub.cpp

	Stub implementations for CG (Client to Game) packet handlers.
	These are needed because the actual handler implementations are server-side.

-----------------------------------------------------------------------------*/

#include "Client_PCH.h"

// Include packet headers to get handler class declarations
// Phase 12 Wave 1 batch 1: CGBuyStoreItem and CGWithdrawPet migrated to
// shared/Packets/. Wave 1 batch 2: CGDisplayItem, CGLotterySelect,
// CGRequestStoreInfo, CGSelectQuest, CGSelectRegenZone, CGStoreClose,
// CGStoreOpen, CGStoreSign, CGUndisplayItem joined them (flat, no
// Cpackets/ subpath, resolved via the shared_packets_cg INTERFACE
// include path); their stubs below dropped the
// throw(ProtocolException, Error) spec to match the migrated header's
// now-unconstrained CGxxxHandler::execute declaration (adopted from the
// server's canonical style, per Phase 12's reconciliation rules) — an
// out-of-line definition's exception spec must match its declaration
// exactly.
#include "CGBuyStoreItem.h"
#include "Packet/Cpackets/CGConnectSetKey.h"
#include "CGDisplayItem.h"
#include "Packet/Cpackets/CGDonationMoney.h"
#include "CGLotterySelect.h"
#include "Packet/Cpackets/CGMixItem.h"
#include "CGRequestStoreInfo.h"
#include "CGSelectQuest.h"
#include "CGSelectRegenZone.h"
#include "CGStoreClose.h"
#include "CGStoreOpen.h"
#include "CGStoreSign.h"
#include "CGUndisplayItem.h"
#include "CGWithdrawPet.h"

// Stub implementations for store-related handlers
void CGBuyStoreItemHandler::execute(CGBuyStoreItem* pPacket, Player* pPlayer) {}
void CGConnectSetKeyHandler::execute(CGConnectSetKey* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGDisplayItemHandler::execute(CGDisplayItem* pPacket, Player* pPlayer) {}
void CGDonationMoneyHandler::execute(CGDonationMoney* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGLotterySelectHandler::execute(CGLotterySelect* pPacket, Player* pPlayer) {}
void CGMixItemHandler::execute(CGMixItem* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGRequestStoreInfoHandler::execute(CGRequestStoreInfo* pPacket, Player* pPlayer) {}
void CGSelectQuestHandler::execute(CGSelectQuest* pPacket, Player* pPlayer) {}
void CGSelectRegenZoneHandler::execute(CGSelectRegenZone* pPacket, Player* pPlayer) {}
void CGStoreCloseHandler::execute(CGStoreClose* pPacket, Player* pPlayer) {}
void CGStoreOpenHandler::execute(CGStoreOpen* pPacket, Player* pPlayer) {}
void CGStoreSignHandler::execute(CGStoreSign* pPacket, Player* pPlayer) {}
void CGUndisplayItemHandler::execute(CGUndisplayItem* pPacket, Player* pPlayer) {}
void CGWithdrawPetHandler::execute(CGWithdrawPet* pPacket, Player* pPlayer) {}
