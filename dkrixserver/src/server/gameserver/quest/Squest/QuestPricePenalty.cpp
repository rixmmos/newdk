//--------------------------------------------------------------------------------
// QuestPricePenalty.h
//--------------------------------------------------------------------------------

#include "QuestPricePenalty.h"

#include "Creature.h"

//--------------------------------------------------------------------------------
// affect ( Creature* )
//--------------------------------------------------------------------------------
void QuestPricePenalty::affect(Creature* pCreature) throw(Error) {
    __BEGIN_TRY

    if (pCreature == NULL)
        return;

    
    
    
    cout << "[Penalty to " << pCreature->getName().c_str() << "] "
         << "Gold=" << (int)getGold() << ", RankExp=" << (int)getRankExp() << ", Fame=" << (int)getFame() << endl;

    

    __END_CATCH
}
