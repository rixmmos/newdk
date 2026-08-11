#include "SMSAddressBook.h"

#include "DB.h"
#include "GCAddressListVerify.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"

AddressUnit* SMSAddressElement::getAddressUnit() const {
    AddressUnit* pRet = new AddressUnit;
    pRet->ElementID = m_ElementID;
    pRet->CharacterName = m_CharacterName;
    pRet->CustomName = m_CustomName;
    pRet->Number = m_Number;

    return pRet;
}

SMSAddressBook::~SMSAddressBook() {
    unordered_map<DWORD, SMSAddressElement*>::iterator itr = m_Addresses.begin();
    unordered_map<DWORD, SMSAddressElement*>::iterator endItr = m_Addresses.end();

    for (; itr != endItr; ++itr) {
        SAFE_DELETE(itr->second);
    }
}

void SMSAddressBook::load() {
    __BEGIN_TRY

    m_Addresses.clear();
    Assert(m_pOwner != NULL);

    m_NextEID = 1;

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectAddressBookStmt(
            pConn, "SELECT eID, CharacterName, CustomName, Number FROM SMSAddressBook WHERE OwnerID=?");
        selectAddressBookStmt.bindString(1, m_pOwner->getName());
        Result* pResult = selectAddressBookStmt.execute();

        while (pResult->next()) {
            SMSAddressElement* pElement = new SMSAddressElement(pResult->getInt(1), pResult->getString(2),
                                                                pResult->getString(3), pResult->getString(4));

            //			Assert( addAddressElement( pElement ) );
            // Use find() rather than operator[]: the latter inserts a default
            // entry, i.e. it mutates the map from inside an Assert, which
            // disappears entirely under NDEBUG.
            Assert(m_Addresses.find(pElement->getID()) == m_Addresses.end());
            m_Addresses[pElement->getID()] = pElement;
            if (m_NextEID <= pElement->getID())
                m_NextEID = pElement->getID() + 1;
        }
    }
    END_DB(pStmt);

    __END_CATCH
}

GCSMSAddressList* SMSAddressBook::getGCSMSAddressList() const {
    GCSMSAddressList* pRet = new GCSMSAddressList;

    unordered_map<DWORD, SMSAddressElement*>::const_iterator itr = m_Addresses.begin();
    unordered_map<DWORD, SMSAddressElement*>::const_iterator endItr = m_Addresses.end();

    for (; itr != endItr; ++itr) {
        if (itr->second != NULL)
            pRet->getAddresses().push_back(itr->second->getAddressUnit());
    }

    return pRet;
}

int SMSAddressBook::addAddressElement(SMSAddressElement* pElement) {
    // ">=" not ">": GCSMSAddressList is declared to hold at most
    // MAX_ADDRESS_NUM units, so a book that reached MAX_ADDRESS_NUM + 1
    // produced a packet larger than its own declared maximum and was dropped
    // by the receiver's size check, making the whole list unreadable.
    if (m_Addresses.size() >= MAX_ADDRESS_NUM)
        return GCAddressListVerify::ADD_FAIL_MAX_NUM_EXCEEDED;
    // find() rather than operator[], which would insert a stray NULL entry
    // and inflate size() on every rejected add.
    if (m_Addresses.find(pElement->getID()) != m_Addresses.end())
        return GCAddressListVerify::ADD_FAIL_INVALID_DATA;

    m_Addresses[pElement->getID()] = pElement;

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertAddressStmt(
            pConn,
            "INSERT INTO SMSAddressBook (eID, OwnerID, CharacterName, CustomName, Number) VALUES (?, ?, ?, ?, ?)");
        insertAddressStmt.bindUInt(1, pElement->m_ElementID);
        insertAddressStmt.bindString(2, m_pOwner->getName());
        insertAddressStmt.bindString(3, pElement->m_CharacterName);
        insertAddressStmt.bindString(4, pElement->m_CustomName);
        insertAddressStmt.bindString(5, pElement->m_Number);
        insertAddressStmt.execute();
    }
    END_DB(pStmt);

    return 0;
}

int SMSAddressBook::removeAddressElement(DWORD eID) {
    unordered_map<DWORD, SMSAddressElement*>::iterator itr = m_Addresses.find(eID);

    if (itr == m_Addresses.end())
        return GCAddressListVerify::DELETE_FAIL_NO_SUCH_EID;

    // The book owns its elements (see the destructor); erasing without
    // deleting leaked one element per client-issued delete.
    SAFE_DELETE(itr->second);
    m_Addresses.erase(itr);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement deleteAddressStmt(pConn, "DELETE FROM SMSAddressBook WHERE eID = ? AND OwnerID = ?");
        deleteAddressStmt.bindUInt(1, eID);
        deleteAddressStmt.bindString(2, m_pOwner->getName());
        deleteAddressStmt.execute();
    }
    END_DB(pStmt);

    return 0;
}
