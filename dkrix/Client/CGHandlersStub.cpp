/*-----------------------------------------------------------------------------

	CGHandlersStub.cpp

	Stub implementations for CG (Client to Game) packet handlers.
	These are needed because the actual handler implementations are server-side.

-----------------------------------------------------------------------------*/

#include "Client_PCH.h"

// Include packet headers to get handler class declarations
#include "CGBuyStoreItem.h"
#include "../shared/Packets/CGConnectSetKey.h"
// Phase 12: CGDisplayItem migrated to shared/Packets/.
#include "CGDisplayItem.h"
#include "../shared/Packets/CGDonationMoney.h"
#include "../shared/Packets/CGLotterySelect.h"
#include "../shared/Packets/CGMixItem.h"
// Phase 12: CGRequestStoreInfo migrated to shared/Packets/.
#include "CGRequestStoreInfo.h"
#include "CGSelectQuest.h"
#include "CGSelectRegenZone.h"
// Phase 12: CGStoreOpen/CGStoreClose migrated to shared/Packets/;
// resolved via shared_packets INTERFACE include path.
#include "CGStoreOpen.h"
#include "CGStoreClose.h"
// Phase 12: CGStoreSign migrated to shared/Packets/ too.
#include "CGStoreSign.h"
#include "CGUndisplayItem.h"
#include "CGWithdrawPet.h"

// Stub implementations for store-related handlers
void CGBuyStoreItemHandler::execute(CGBuyStoreItem* pPacket, Player* pPlayer) {}
void CGConnectSetKeyHandler::execute(CGConnectSetKey* pPacket, Player* pPlayer) {}
void CGDisplayItemHandler::execute(CGDisplayItem* pPacket, Player* pPlayer) {}
void CGDonationMoneyHandler::execute(CGDonationMoney* pPacket, Player* pPlayer) {}
void CGLotterySelectHandler::execute(CGLotterySelect* pPacket, Player* pPlayer) {}
void CGMixItemHandler::execute(CGMixItem* pPacket, Player* pPlayer) {}
void CGRequestStoreInfoHandler::execute(CGRequestStoreInfo* pPacket, Player* pPlayer) {}
void CGSelectQuestHandler::execute(CGSelectQuest* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGSelectRegenZoneHandler::execute(CGSelectRegenZone* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
// Phase 12: CGBuyStoreItem/CGDisplayItem/CGStoreOpen/CGStoreClose/
// CGStoreSign/CGRequestStoreInfo/CGUndisplayItem headers migrated to
// shared/Packets/,
// which drops the throw() specs (deprecated in C++17). Match the new
// signatures here.
void CGStoreOpenHandler::execute(CGStoreOpen* pPacket, Player* pPlayer) {}
void CGStoreCloseHandler::execute(CGStoreClose* pPacket, Player* pPlayer) {}
void CGStoreSignHandler::execute(CGStoreSign* pPacket, Player* pPlayer) {}
void CGUndisplayItemHandler::execute(CGUndisplayItem* pPacket, Player* pPlayer) {}
void CGWithdrawPetHandler::execute(CGWithdrawPet* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
