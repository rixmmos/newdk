
#include "EventQuestRewardManager.h"

#include "DB.h"
#include "Item.h"
#include "ItemRewardInfo.h"
#include "QuestAdvanceRewardInfo.h"
#include "RandomRewardClass.h"
#include "SlayerWeaponRewardClass.h"

void EventQuestRewardManager::load(const string& name) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        

         

         

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_CATCH
}
