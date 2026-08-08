//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemIDRegistry.cpp
// Written by  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#include "AR.h"
#include "Belt.h"
#include "Blade.h"
#include "BloodBible.h"
#include "Bomb.h"
#include "BombMaterial.h"
#include "Bracelet.h"
#include "CarryingReceiver.h"
#include "CastleSymbol.h"
#include "Coat.h"
#include "CodeSheet.h"
#include "ComposMei.h"
#include "CoreZap.h"
#include "CoupleRing.h"
#include "Cross.h"
#include "DB.h"
#include "Dermis.h"
#include "DyePotion.h"
#include "ETC.h"
#include "EffectItem.h"
#include "EventETC.h"
#include "EventGiftBox.h"
#include "EventItem.h"
#include "EventStar.h"
#include "EventTree.h"
#include "Fascia.h"
#include "Glove.h"
#include "Helm.h"
#include "HolyWater.h"
#include "ItemInfoManager.h"
#include "Key.h"
#include "Larva.h"
#include "LearningItem.h"
#include "LuckyBag.h"
#include "Mace.h"
#include "Magazine.h"
#include "Mine.h"
#include "Mitten.h"
#include "MixingItem.h"
#include "Money.h"
#include "MoonCard.h"
#include "Motorcycle.h"
#include "Necklace.h"
#include "OustersArmsband.h"
#include "OustersBoots.h"
#include "OustersChakram.h"
#include "OustersCirclet.h"
#include "OustersCoat.h"
#include "OustersPendent.h"
#include "OustersRing.h"
#include "OustersStone.h"
#include "OustersSummonItem.h"
#include "OustersWristlet.h"
#include "Persona.h"
#include "PetEnchantItem.h"
#include "PetFood.h"
#include "PetItem.h"
#include "Potion.h"
#include "PreparedStatement.h"
#include "Pupa.h"
#include "QuestItem.h"
#include "Relic.h"
#include "ResurrectItem.h"
#include "Ring.h"
#include "SG.h"
#include "SMG.h"
#include "SMSItem.h"
#include "SR.h"
#include "Serum.h"
#include "Shield.h"
#include "Shoes.h"
#include "ShoulderArmor.h"
#include "Skull.h"
#include "SlayerPortalItem.h"
#include "Sweeper.h"
#include "Sword.h"
#include "TrapItem.h"
#include "Trouser.h"
#include "VampireAmulet.h"
#include "VampireBracelet.h"
#include "VampireCoat.h"
#include "VampireCoupleRing.h"
#include "VampireETC.h"
#include "VampireEarring.h"
#include "VampireNecklace.h"
#include "VampirePortalItem.h"
#include "VampireRing.h"
#include "VampireWeapon.h"
#include "WarItem.h"
#include "Water.h"

#define ITEMIDREGISTRY(CLASS, TABLE)                                                              \
                                                                                                  \
    void CLASS::initItemIDRegistry(void)                                                          \
                                                                                                  \
    {                                                                                             \
        __BEGIN_TRY                                                                               \
                                                                                                  \
        Statement* pStmt = NULL;                                                                  \
                                                                                                  \
        __ENTER_CRITICAL_SECTION(m_Mutex)                                                         \
                                                                                                  \
        BEGIN_DB {                                                                                \
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");                    \
            PreparedStatement countStmt(pConn, "SELECT COUNT(*) from " TABLE);                    \
            Result* pCountResult = countStmt.execute();                                           \
            pCountResult->next();                                                                 \
            int count = pCountResult->getDWORD(1);                                                \
                                                                                                  \
            if (count != 0) {                                                                     \
                PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM " TABLE);             \
                Result* pResult = maxIDStmt.execute();                                            \
                pResult->next();                                                                  \
                m_ItemIDRegistry = pResult->getDWORD(1);                                          \
            }                                                                                     \
                                                                                                  \
            m_ItemIDRegistry += (g_pItemInfoManager->getItemIDSuccessor() -                       \
                                 (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) + \
                                g_pItemInfoManager->getItemIDBase();                              \
        }                                                                                         \
        END_DB(pStmt)                                                                             \
                                                                                                  \
        __LEAVE_CRITICAL_SECTION(m_Mutex)                                                         \
                                                                                                  \
        cout << TABLE << "...ItemIDRegistry:" << m_ItemIDRegistry << endl;                        \
                                                                                                  \
        __END_CATCH                                                                               \
    }

ITEMIDREGISTRY(CarryingReceiver, "CarryingReceiverObject");
ITEMIDREGISTRY(ShoulderArmor, "ShoulderArmorObject");
ITEMIDREGISTRY(Dermis, "DermisObject");
ITEMIDREGISTRY(Persona, "PersonaObject");
ITEMIDREGISTRY(Fascia, "FasciaObject");
ITEMIDREGISTRY(Mitten, "MittenObject");

void Motorcycle::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from MotorcycleObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM MotorcycleObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Motorcycle...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Potion::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        // pConn = g_pDatabaseManager->getConnection("DIST_DARKEDEN"); (dead, both queries used it)
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from PotionObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM PotionObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Potion...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Water::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from WaterObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM WaterObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Water...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void HolyWater::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from HolyWaterObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM HolyWaterObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "HolyWater...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Magazine::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from MagazineObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM MagazineObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Magazine...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void BombMaterial::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from BombMaterialObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM BombMaterialObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "BombMaterial...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void ETC::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from ETCObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM ETCObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "ETC...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Key::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from KeyObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM KeyObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Key...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Ring::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from RingObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM RingObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Ring...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Bracelet::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from BraceletObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM BraceletObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Bracelet...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Necklace::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from NecklaceObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM NecklaceObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Necklace...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Coat::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from CoatObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM CoatObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Coat...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Trouser::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from TrouserObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM TrouserObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Trouser...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Shoes::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from ShoesObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM ShoesObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Shoes...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Sword::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SwordObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SwordObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Sword...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Blade::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from BladeObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM BladeObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Blade...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Shield::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from ShieldObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM ShieldObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Shield...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Cross::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from CrossObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM CrossObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Cross...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Mace::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from MaceObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM MaceObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Mace...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Glove::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from GloveObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM GloveObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Glove...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Helm::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from HelmObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM HelmObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Helm...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void SG::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SGObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SGObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "SG...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void SMG::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SMGObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SMGObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "SMG...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void AR::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from ARObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM ARObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "AR...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void SR::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SRObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SRObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "SR...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Bomb::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from BombObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM BombObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Bomb...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Mine::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from MineObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM MineObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Mine...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Belt::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from BeltObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM BeltObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Belt...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void LearningItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from LearningItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM LearningItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "LearningItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Money::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from MoneyObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM MoneyObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Money...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireRing::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireRingObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireRingObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireRing...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}


void VampireBracelet::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireBraceletObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireBraceletObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireBracelet...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireNecklace::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireNecklaceObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireNecklaceObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireNecklace...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireCoat::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireCoatObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireCoatObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireCoat...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Skull::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SkullObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SkullObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Skull...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Serum::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SerumObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SerumObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Serum...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireETC::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireETCObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireETCObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireETC...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void SlayerPortalItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SlayerPortalItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SlayerPortalItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "SlayerPortalItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampirePortalItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampirePortalItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampirePortalItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampirePortalItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void EventGiftBox::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from EventGiftBoxObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM EventGiftBoxObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "GiftBox...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void EventStar::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from EventStarObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM EventStarObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "EventStar...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireEarring::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireEarringObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireEarringObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireEarring...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Relic::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from RelicObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM RelicObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "RelicObject...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireWeapon::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireWeaponObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireWeaponObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireWeaponObject...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireAmulet::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireAmuletObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireAmuletObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "VampireAmuletObject...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void QuestItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from QuestItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM QuestItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "QuestItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void EventTree::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from EventTreeObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM EventTreeObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "EventTree...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void EventETC::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from EventETCObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM EventETCObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "EventETC...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void BloodBible::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from BloodBibleObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM BloodBibleObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "BloodBible...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void CastleSymbol::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from CastleSymbolObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM CastleSymbolObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "CastleSymbol...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void CoupleRing::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from CoupleRingObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM CoupleRingObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "CoupleRing...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void VampireCoupleRing::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from VampireCoupleRingObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM VampireCoupleRingObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "CoupleRing...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void EventItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from EventItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM EventItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "EventItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void DyePotion::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from DyePotionObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM DyePotionObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "DyePotion...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void ResurrectItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from ResurrectItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM ResurrectItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "ResurrectItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void MixingItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from MixingItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM MixingItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "MixingItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersArmsband::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersArmsbandObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersArmsbandObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersArmsband...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersBoots::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersBootsObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersBootsObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersBoots...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersChakram::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersChakramObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersChakramObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersChakram...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersCirclet::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersCircletObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersCircletObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersCirclet...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersCoat::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersCoatObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersCoatObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersCoat...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersPendent::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersPendentObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersPendentObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersPendent...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersRing::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersRingObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersRingObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersRing...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersStone::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersStoneObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersStoneObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersStone...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersWristlet::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersWristletObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersWristletObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersWristlet...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Larva::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from LarvaObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM LarvaObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Larva...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Pupa::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from PupaObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM PupaObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Pupa...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void ComposMei::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from ComposMeiObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM ComposMeiObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "ComposMei...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void OustersSummonItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from OustersSummonItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM OustersSummonItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "OustersSummonItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void EffectItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from EffectItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM EffectItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "EffectItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void CodeSheet::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from CodeSheetObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM CodeSheetObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "CodeSheet...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void MoonCard::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from MoonCardObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM MoonCardObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "MoonCard...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void Sweeper::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SweeperObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SweeperObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "Sweeper...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void PetItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from PetItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM PetItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "PetItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void PetFood::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from PetFoodObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM PetFoodObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "PetFood...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void PetEnchantItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from PetEnchantItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM PetEnchantItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "PetEnchantItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void LuckyBag::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from LuckyBagObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM LuckyBagObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "LuckyBag...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void SMSItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from SMSItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM SMSItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "SMSItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void CoreZap::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from CoreZapObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM CoreZapObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "CoreZap...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void TrapItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from TrapItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM TrapItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "TrapItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}

void WarItem::initItemIDRegistry(void)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement countStmt(pConn, "SELECT COUNT(*) from WarItemObject");
        Result* pCountResult = countStmt.execute();
        pCountResult->next();
        int count = pCountResult->getDWORD(1);

        if (count != 0) {
            PreparedStatement maxIDStmt(pConn, "SELECT MAX(ItemID) FROM WarItemObject");
            Result* pResult = maxIDStmt.execute();
            pResult->next();
            m_ItemIDRegistry = pResult->getDWORD(1);
        }

        m_ItemIDRegistry +=
            (g_pItemInfoManager->getItemIDSuccessor() - (m_ItemIDRegistry % g_pItemInfoManager->getItemIDSuccessor())) +
            g_pItemInfoManager->getItemIDBase();
    }
    END_DB(pStmt)

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    cout << "WarItem...ItemIDRegistry:" << m_ItemIDRegistry << endl;

    __END_CATCH
}
