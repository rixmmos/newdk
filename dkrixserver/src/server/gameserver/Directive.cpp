////////////////////////////////////////////////////////////////////////
// Filename    : Directive.cpp

// Description :


////////////////////////////////////////////////////////////////////////

#include "Directive.h"

#include <stdio.h>
#include <string.h>

#include "Assert.h"
#include "DB.h"
#include "StringStream.h"
#include "skill/Skill.h"


#define MODE_CONDITION 0
#define MODE_ACTION 1


DirectiveSetManager* g_pDirectiveSetManager = NULL;

////////////////////////////////////////////////////////////////////////
// class Directive member methods
////////////////////////////////////////////////////////////////////////

Directive::Directive()

{
    __BEGIN_TRY

    m_Action = 0;
    m_Parameter = 0;
    m_Ratio = 0;
    m_Weight = 0;

    __END_CATCH
}

Directive::~Directive()

{
    __BEGIN_TRY

    m_Conditions.clear();

    __END_CATCH_NO_RETHROW
}

void Directive::addCondition(int condition)

{
    __BEGIN_TRY

    
    list<int>::iterator itr = m_Conditions.begin();
    for (; itr != m_Conditions.end(); itr++) {
        if (condition == *itr)
            return;
    }

    m_Conditions.push_back(condition);

    __END_CATCH
}

string Directive::toString()

{
    __BEGIN_TRY

    StringStream msg;

    // header
    msg << "Directive(";

    // condition output
    msg << "Condition : ";
    list<int>::const_iterator itr = m_Conditions.begin();
    for (; itr != m_Conditions.end(); itr++)
        msg << DirectiveCondition2String[*itr] << ",";
    msg << " ";

    // action output
    msg << "Action : " << DirectiveAction2String[m_Action] << " ";

    // parameter output
    if (m_Parameter != 0)
        msg << "Parameter : " << SkillTypes2String[m_Parameter] << " ";

    // ratio output
    if (m_Ratio != 0)
        msg << "Ratio : " << m_Ratio;

    // tail
    msg << ")";

    return msg.toString();

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// class DirectiveSet member methods
////////////////////////////////////////////////////////////////////////

DirectiveSet::DirectiveSet()

    {__BEGIN_TRY

         __END_CATCH}

DirectiveSet::~DirectiveSet()

{
    __BEGIN_TRY

    
    list<Directive*>::iterator itr = m_Directives.begin();
    for (; itr != m_Directives.end(); itr++) {
        Directive* pDirective = *itr;
        SAFE_DELETE(pDirective);
    }

    m_Directives.clear();

    __END_CATCH_NO_RETHROW
}

void DirectiveSet::clear()

{
    __BEGIN_TRY

    
    list<Directive*>::iterator itr = m_Directives.begin();
    for (; itr != m_Directives.end(); itr++) {
        Directive* pDirective = *itr;
        SAFE_DELETE(pDirective);
    }

    m_Directives.clear();

    __END_CATCH
}

bool DirectiveSet::hasCondition(int condition) const {
    list<Directive*>::const_iterator itr = m_Directives.begin();
    list<Directive*>::const_iterator endItr = m_Directives.end();

    for (; itr != endItr; ++itr) {
        if ((*itr)->hasCondition(condition))
            return true;
    }

    return false;
}

void DirectiveSet::addDirective(Directive* pDirective)

{
    __BEGIN_TRY

    m_Directives.push_back(pDirective);

    __END_CATCH
}

void DirectiveSet::addDeadDirective(Directive* pDirective)

{
    __BEGIN_TRY

    m_DeadDirectives.push_back(pDirective);

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
DirectiveSetManager::DirectiveSetManager()

{
    __BEGIN_TRY

    m_nSetCount = 0;
    m_ppSet = NULL;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
DirectiveSetManager::~DirectiveSetManager()

{
    __BEGIN_TRY

    if (m_ppSet) {
        for (uint i = 0; i < m_nSetCount; i++) {
            SAFE_DELETE(m_ppSet[i]);
        }

        SAFE_DELETE_ARRAY(m_ppSet); 
    }

    __END_CATCH_NO_RETHROW
}

////////////////////////////////////////////////////////////////////////
// init()

////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::init()

{
    __BEGIN_TRY

    load();

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////
// load()


////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        
        pResult = pStmt->executeQuery("SELECT MAX(ID) FROM DirectiveSet");

        if (pResult->getRowCount() == 0)
            throw("DirectiveSetManager::load() :  .");

        pResult->next();
        m_nSetCount = pResult->getInt(1) + 1;

        
        if (m_ppSet == NULL) {
            m_ppSet = new DirectiveSet*[m_nSetCount];
            Assert(m_ppSet != NULL);
            for (uint i = 0; i < m_nSetCount; i++) {
                m_ppSet[i] = NULL;
            }
        }

        
        pResult = pStmt->executeQuery("SELECT ID, Name, Content, DeadContent FROM DirectiveSet");

        while (pResult->next()) {
            uint index = pResult->getInt(1);
            string name = pResult->getString(2);
            string text = pResult->getString(3);
            string deadtext = pResult->getString(4);

            printf("ID[%d] Directive Loading Begin >> ", index);

            ////////////////
            /// filelog("Directive.txt", "===============================================");
            // filelog("Directive.txt", "Name:%s", name.c_str());
            ////////////////

            createDirectiveSet(index, name, text, deadtext);

            printf("Loading End\n");
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getDirectiveSet()

////////////////////////////////////////////////////////////////////////
DirectiveSet* DirectiveSetManager::getDirectiveSet(uint index)

{
    __BEGIN_TRY

    if (index >= m_nSetCount) {
        cerr << "DirectiveSetManager::getDirectiveSet() : Out of bounds, value = [" << index << "]" << endl;
        throw("DirectiveSetManager::getDirectiveSet() : Out of bounds!");
    }

    return m_ppSet[index];

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// createDirectiveSet()

////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::createDirectiveSet(uint index, const string& name, const string& text, const string& deadtext)

{
    __BEGIN_TRY

    if (index >= m_nSetCount) {
        cerr << "DirectiveSetManager::createDirectiveSet() : Out of bounds!" << endl;
        throw("DirectiveSetManager::createDirectiveSet() : Out of bounds!");
    }

    
    if (m_ppSet[index] == NULL) // by sigi. 2002.9.26
    {
        // SAFE_DELETE(m_ppSet[index]);
        m_ppSet[index] = new DirectiveSet;
    } else {
        
        m_ppSet[index]->clear();
    }

    m_ppSet[index]->setName(name);

    string beginToken = "DIRECTIVE BEGIN";
    string endToken = "DIRECTIVE END";
    uint start = 0;
    uint end = 0;

    while (end < text.size() - 1) {
        ////////////////////////////////////////////////////////////
        // ex)
        // DIRECTIVE BEGIN
        //   (CONDITION:ImDying)
        //   (ACTION:Flee)
        // DIRECTIVE END
        // DIRECTIVE BEGIN
        //   (CONDITION:EnemyRangeMelee)
        //   (CONDITION:EnemyDying)
        //   (CONDITION:EnemyNotBloodDrained)
        //   (ACTION:UseSkill,BloodDrain,30)
        // DIRECTIVE END
        ////////////////////////////////////////////////////////////

        start = text.find(beginToken, end);
        end = text.find(endToken, start + 1);

        if (start > end)
            break;

        Directive* pDirective = new Directive;
        string directive = trim(text.substr(start + beginToken.size() + 1, end - start - beginToken.size() - 1));

        Assert(pDirective != NULL);

        size_t i = 0, j = 0, k = 0;

        while (k < directive.size()) {
            
            
            i = directive.find_first_of('(', k);
            j = directive.find_first_of(':', i + 1);
            k = directive.find_first_of(')', j + 1);

            
            if (i > j || j > k)
                break;

            
            
            string identifier = directive.substr(i + 1, j - i - 1);
            string parameter = directive.substr(j + 1, k - j - 1);

            if (identifier == "CONDITION") {
                parseDirectiveParameter(pDirective, parameter, MODE_CONDITION);
            } else if (identifier == "ACTION") {
                parseDirectiveParameter(pDirective, parameter, MODE_ACTION);
            } else {
                cerr << "DirectiveSetManager::createDirectiveSet() :    IDENTIFIER" << endl;
                throw("DirectiveSetManager::createDirectiveSet() :    IDENTIFIER");
            }
        }

        m_ppSet[index]->addDirective(pDirective);
    }

    
    
    
    
    if (deadtext.size() > 5) {
        start = 0;
        end = 0;

        while (end < deadtext.size() - 1) {
            ////////////////////////////////////////////////////////////
            // ex)
            // DIRECTIVE BEGIN
            //   (CONDITION:ImDying)
            //   (ACTION:Flee)
            // DIRECTIVE END
            // DIRECTIVE BEGIN
            //   (CONDITION:EnemyRangeMelee)
            //   (CONDITION:EnemyDying)
            //   (CONDITION:EnemyNotBloodDrained)
            //   (ACTION:UseSkill,BloodDrain,30)
            // DIRECTIVE END
            ////////////////////////////////////////////////////////////

            start = deadtext.find(beginToken, end);
            end = deadtext.find(endToken, start + 1);

            if (start > end)
                break;

            Directive* pDirective = new Directive;
            string directive =
                trim(deadtext.substr(start + beginToken.size() + 1, end - start - beginToken.size() - 1));

            Assert(pDirective != NULL);

            size_t i = 0, j = 0, k = 0;

            while (k < directive.size()) {
                
                
                i = directive.find_first_of('(', k);
                j = directive.find_first_of(':', i + 1);
                k = directive.find_first_of(')', j + 1);

                
                if (i > j || j > k)
                    break;

                
                
                string identifier = directive.substr(i + 1, j - i - 1);
                string parameter = directive.substr(j + 1, k - j - 1);

                if (identifier == "CONDITION") {
                    parseDirectiveParameter(pDirective, parameter, MODE_CONDITION);
                } else if (identifier == "ACTION") {
                    parseDirectiveParameter(pDirective, parameter, MODE_ACTION);
                } else {
                    cerr << "DirectiveSetManager::createDirectiveSet() :    IDENTIFIER" << endl;
                    throw("DirectiveSetManager::createDirectiveSet() :    IDENTIFIER");
                }
            }

            m_ppSet[index]->addDeadDirective(pDirective);
        }
    }

    m_ppSet[index]->setAttackAir(m_ppSet[index]->hasCondition(DIRECTIVE_COND_ENEMY_ON_AIR));
    m_ppSet[index]->setSeeSafeZone(m_ppSet[index]->hasCondition(DIRECTIVE_COND_ENEMY_ON_SAFE_ZONE));

    if (m_ppSet[index]->canSeeSafeZone())
        cout << index << " : can see safe zone" << endl;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// parseDirectiveParameter()

////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::parseDirectiveParameter(Directive* pDirective, const string& text, int mode)

{
    __BEGIN_TRY

    size_t oldpos = 0;
    size_t pos = 0;
    size_t paramCount = 0;

    while (oldpos < text.size() && pos < text.size()) {
        
        oldpos = pos;
        pos = text.find_first_of(',', oldpos);

        
        
        
        
        if (pos == string::npos)
            pos = text.size();
        pos++;

        string token = text.substr(oldpos, (pos - 1) - oldpos);

        switch (mode) {
        case MODE_CONDITION:
            if (paramCount != 0) {
                cerr << "DirectiveSetManager::parseParameter() : There's two or more parameters in condition" << endl;
                cerr << "ParsingText: " << text.c_str() << endl;
                throw("DirectiveSetManager::parseParameter() : There's two or more parameters in condition");
            }
            pDirective->addCondition(getCondition(token));
            break;
        case MODE_ACTION:
            if (paramCount == 0)
                pDirective->setAction(getAction(token));
            else if (paramCount == 1) {
                switch (pDirective->getAction()) {
                case DIRECTIVE_ACTION_USE_SKILL:
                    pDirective->setParameter(getParameter(token));
                    break;

                case DIRECTIVE_ACTION_CHANGE_ENEMY:
                case DIRECTIVE_ACTION_MOVE_RANDOM:
                case DIRECTIVE_ACTION_SAY:
                    pDirective->setParameter(atoi(token.c_str()));
                    break;


                default:
                    cerr << "DirectiveSetManager::parseParameter() : There's two or more parameters in action" << endl;
                    cerr << "ParsingText: " << text.c_str() << endl;
                    throw Error("No Need Parameter!");
                }

                ////////////////
                // if (pDirective->getAction() == DIRECTIVE_ACTION_USE_SKILL)
                //{
                //	filelog("Directive.txt", "%s", token.c_str());
                // }
                ////////////////
            } else if (paramCount == 2)
                pDirective->setRatio(getRatio(token));
            break;
        default:
            break;
        }
        paramCount++;
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getCondition()

////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getCondition(const string& token)

{
    __BEGIN_TRY

    for (int i = 0; i < DIRECTIVE_COND_MAX; i++)
        if (DirectiveCondition2String[i] == token)
            return i;

    StringStream msg;
    msg << "DirectiveSetManager::getCondition() : Invalid condition string[" << token << "]";

    cerr << msg.toString() << endl;
    throw(msg.toString());
    return DIRECTIVE_COND_MAX;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getAction()

////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getAction(const string& token)

{
    __BEGIN_TRY

    for (int i = 0; i < DIRECTIVE_ACTION_MAX; i++)
        if (DirectiveAction2String[i] == token)
            return i;

    cerr << "DirectiveSetManager::getAction() : Invalid action[" << token << "]" << endl;
    throw("DirectiveSetManager::getAction() : Invalid action");
    return DIRECTIVE_COND_MAX;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getParameter()


////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getParameter(const string& token)

{
    __BEGIN_TRY

    for (int i = 0; i < SKILL_MAX; i++) {
        if (strcasecmp(token.c_str(), SkillTypes2String[i].c_str()) == 0)
            return i;
    }

    cerr << "DirectiveSetManager::getParameter() : Invalid parameter[" << token << "]" << endl;
    throw("DirectiveSetManager::getParameter() : Invalid parameter");

    return 0;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getRatio()

////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getRatio(const string& token)

{
    __BEGIN_TRY

    int rValue = atoi(token.c_str());

    if (rValue < 0 || rValue > 100) {
        cerr << "DirectiveSetManager::getRatio() : Invalid ratio value" << endl;
        throw("DirectiveSetManager::getRatio() : Invalid ratio value");
    }

    return rValue;

    __END_CATCH
}
