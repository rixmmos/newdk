//--------------------------------------------------------------------------------
// QuestPriceReward.h
//--------------------------------------------------------------------------------

#include "QuestPriceReward.h"

#include "Creature.h"

//--------------------------------------------------------------------------------
// affect ( Creature* )
//--------------------------------------------------------------------------------
void QuestPriceReward::affect(Creature* pCreature) throw(Error) {
    __BEGIN_TRY

    if (pCreature == NULL)
        return;

    
    
    
    cout << "[Reward to " << pCreature->getName().c_str() << "] "
         << "Gold=" << (int)getGold() << ", RankExp=" << (int)getRankExp() << ", Fame=" << (int)getFame() << endl;

    

    __END_CATCH
}
