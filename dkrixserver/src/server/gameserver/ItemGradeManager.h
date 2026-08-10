#ifndef __ITEM_GRADE_MANAGER_H__
#define __ITEM_GRADE_MANAGER_H__

#include <vector>

#include "Exception.h"
#include "Types.h"

class ItemGradeManager {
public:
    ItemGradeManager() {
        // assign, not reserve: reserve() leaves size() at 0, so load()'s
        // m_GradeXxx[Grade - 1] writes were indexing an empty container.
        // All three are sized here, not just m_GradeRatios -- the other two
        // were unsized until load() ran, so getRandomGambleGrade() /
        // getRandomBeadGrade() would have read out of bounds before that.
        m_GradeRatios.assign(10, 0);
        m_GradeGambleRatios.assign(10, 0);
        m_GradeBeadRatios.assign(10, 0);
    }
    void load();
    Grade_t getRandomGrade() const;
    Grade_t getRandomGambleGrade() const;
    Grade_t getRandomBeadGrade() const;

    static ItemGradeManager& Instance();

private:
    vector<WORD> m_GradeRatios;
    vector<WORD> m_GradeGambleRatios;
    vector<WORD> m_GradeBeadRatios;
};

#endif
