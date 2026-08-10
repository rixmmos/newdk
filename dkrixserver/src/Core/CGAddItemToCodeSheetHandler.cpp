//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddItemToCodeSheetHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <list>
#include <vector>

#include "CGAddItemToCodeSheet.h"
#include "GCCannotUse.h"
#include "GCUseOK.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemUtil.h"
#include "PlayerCreature.h"

// The code sheet is a fixed 10x6 grid of stone cells packed two cells per
// byte, so its option-type vector holds exactly 30 entries. Both figures come
// from CodeSheet::CodeSheet(), which builds the vector, and from canPutStone()
// below, whose neighbour walk stops at the last column and row.
const uint CODE_SHEET_WIDTH = 10;
const uint CODE_SHEET_HEIGHT = 6;
const uint CODE_SHEET_OPTION_COUNT = CODE_SHEET_WIDTH * CODE_SHEET_HEIGHT / 2;

uint getStoneNum(const vector<OptionType_t>& OptionType, CoordInven_t x, CoordInven_t y);
void setStoneNum(vector<OptionType_t>& OptionType, CoordInven_t x, CoordInven_t y, uint Num);
bool canPutStone(const vector<OptionType_t>& OptionType, CoordInven_t x, CoordInven_t y, uint StoneNum);
bool isComplete(const vector<OptionType_t>& OptionType);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGAddItemToCodeSheetHandler::execute(CGAddItemToCodeSheet* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    CoordInven_t x, y;
    Item* pMouseItem = pPC->getExtraInventorySlotItem();
    Item* pTargetItem = pPC->getInventory()->findItemOID(pPacket->getObjectID(), x, y);

    GCCannotUse failpkt;
    failpkt.setObjectID(pPacket->getObjectID());

    if (pMouseItem == NULL || pTargetItem == NULL || pMouseItem->getItemClass() != Item::ITEM_CLASS_EVENT_ITEM ||
        pTargetItem->getItemClass() != Item::ITEM_CLASS_CODE_SHEET || pMouseItem->getItemType() > 26 ||
        pMouseItem->getItemType() < 22 || pTargetItem->getItemType() != 0) {
        pPlayer->sendPacket(&failpkt);
        return;
    }

    GCUseOK okpkt;
    uint StoneNum = pMouseItem->getItemType() - 21;
    Assert(StoneNum >= 1 && StoneNum <= 5);

    x = pPacket->getX();
    y = pPacket->getY();

    // SECURITY: x and y arrive straight off the wire as CoordInven_t, a BYTE,
    // so 0-255 each. getStoneNum()/setStoneNum() index the option vector at
    // (y * CODE_SHEET_WIDTH + x) / 2, which reaches 1402 for the worst pair,
    // and the only guard used to be the lower bound on the vector's size below
    // -- an out-of-bounds read in getStoneNum() and an out-of-bounds write in
    // setStoneNum(). Validate the coordinates once here, at packet entry, so
    // every helper is only ever reached with in-grid values. A real runtime
    // check rather than Assert(), so it survives a Release build.
    if (x >= CODE_SHEET_WIDTH || y >= CODE_SHEET_HEIGHT) {
        pPlayer->sendPacket(&failpkt);
        return;
    }

    vector<OptionType_t> OptionType(pTargetItem->getOptionTypeList().begin(), pTargetItem->getOptionTypeList().end());
    if (OptionType.size() < CODE_SHEET_OPTION_COUNT) {
        pPlayer->sendPacket(&failpkt);
        return;
    }

    uint TargetStoneNum = getStoneNum(OptionType, x, y);
    if (TargetStoneNum != 0xf || !canPutStone(OptionType, x, y, StoneNum)) {
        pPlayer->sendPacket(&failpkt);
        return;
    }

    setStoneNum(OptionType, x, y, StoneNum);

    if (isComplete(OptionType)) {
        pTargetItem->setItemType(1);
        pTargetItem->tinysave("ItemType=1");
    }

    pTargetItem->setOptionType(list<OptionType_t>(OptionType.begin(), OptionType.end()));

    string optionField;
    setOptionTypeToField(pTargetItem->getOptionTypeList(), optionField);

    char query[100];
    sprintf(query, "OptionType = '%s'", optionField.c_str());
    pTargetItem->tinysave(query);

    pPC->deleteItemFromExtraInventorySlot();
    pMouseItem->destroy();
    SAFE_DELETE(pMouseItem);

    pPlayer->sendPacket(&okpkt);

    __END_DEBUG_EX __END_CATCH
}

uint getStoneNum(const vector<OptionType_t>& OptionType, CoordInven_t x, CoordInven_t y) {
    uint SerialNum = y * CODE_SHEET_WIDTH + x;
    uint IndexNum = SerialNum / 2;
    uint LowerBit = SerialNum % 2;

    OptionType_t targetOption = OptionType[IndexNum];
    uint TargetStoneNum;

    if (LowerBit) {
        TargetStoneNum = (targetOption & 0x0f);
    } else {
        TargetStoneNum = (targetOption & 0xf0) >> 4;
    }

    return TargetStoneNum;
}

void setStoneNum(vector<OptionType_t>& OptionType, CoordInven_t x, CoordInven_t y, uint StoneNum) {
    uint SerialNum = y * CODE_SHEET_WIDTH + x;
    uint IndexNum = SerialNum / 2;
    uint LowerBit = SerialNum % 2;

    OptionType_t targetOption = OptionType[IndexNum];

    if (LowerBit) {
        targetOption &= 0xf0;
        targetOption |= StoneNum;
    } else {
        targetOption &= 0x0f;
        targetOption |= (StoneNum << 4);
    }

    OptionType[IndexNum] = targetOption;
}

bool canPutStone(const vector<OptionType_t>& OptionType, CoordInven_t x, CoordInven_t y, uint StoneNum) {
    if (x > 0) {
        if (getStoneNum(OptionType, x - 1, y) == StoneNum)
            return false;
    }

    if (y > 0) {
        if (getStoneNum(OptionType, x, y - 1) == StoneNum)
            return false;
    }

    if (x < CODE_SHEET_WIDTH - 1) {
        if (getStoneNum(OptionType, x + 1, y) == StoneNum)
            return false;
    }

    if (y < CODE_SHEET_HEIGHT - 1) {
        if (getStoneNum(OptionType, x, y + 1) == StoneNum)
            return false;
    }

    return true;
}

bool isComplete(const vector<OptionType_t>& OptionType) {
    for (uint i = 0; i < OptionType.size(); ++i) {
        if ((OptionType[i] & 0xf0) == 0xf0 || (OptionType[i] & 0x0f) == 0x0f)
            return false;
    }

    return true;
}
