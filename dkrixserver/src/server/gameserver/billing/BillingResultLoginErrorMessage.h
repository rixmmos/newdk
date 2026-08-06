#ifndef __BILLING_RESULT_LOGIN_ERROR_MESSAGE_H__
#define __BILLING_RESULT_LOGIN_ERROR_MESSAGE_H__

#include <string>

#include <unordered_map>

#include "BillingInfo.h"
#include "Types.h"
#include "Utility.h"

class BillingResultLoginErrorMessage {
public:
    static BillingResultLoginErrorMessage* Instance();

    string getMessage(int index) const;

protected:
    BillingResultLoginErrorMessage();

    void initMessage();

private:
    static BillingResultLoginErrorMessage* _instance;

    typedef unordered_map<int, string> HashMapMessage;
    typedef HashMapMessage::iterator HashMapMessageItr;
    typedef HashMapMessage::const_iterator HashMapMessageConstItr;

    HashMapMessage m_Messages;
};


BillingResultLoginErrorMessage* BillingResultLoginErrorMessage::_instance = 0;

BillingResultLoginErrorMessage* BillingResultLoginErrorMessage::Instance() {
    if (_instance == 0) {
        _instance = new BillingResultLoginErrorMessage;
    }

    return _instance;
}

BillingResultLoginErrorMessage::BillingResultLoginErrorMessage() {
    initMessage();
}

void BillingResultLoginErrorMessage::initMessage() {
    m_Messages[BILLING_RESULT_LOGIN_DB_ERROR] = "DB   .";
    m_Messages[BILLING_RESULT_LOGIN_NETWORK_ERROR] = "  .";
    m_Messages[BILLING_RESULT_LOGIN_NO_CASH] = " .";
    m_Messages[BILLING_RESULT_LOGIN_NO_SESSION] = " .";
    m_Messages[BILLING_RESULT_LOGIN_BAD_PACKET] = "   ";
    m_Messages[BILLING_RESULT_LOGIN_COM_ERROR] = "COM ";
    m_Messages[BILLING_RESULT_LOGIN_NO_RESPONSE] = "   .";
    m_Messages[BILLING_RESULT_LOGIN_NO_MACHINE] = "    .";
    m_Messages[BILLING_RESULT_LOGIN_BAD_GAME_NO] = "    .";
    m_Messages[BILLING_RESULT_LOGIN_ACCOUNT_ERROR] = "   ";
    m_Messages[BILLING_RESULT_LOGIN_DENY] = "    (    )";
    m_Messages[BILLING_RESULT_LOGIN_TIME_OVER] = "  ";
    m_Messages[BILLING_RESULT_LOGIN_BUSY] = "   .";
    m_Messages[BILLING_RESULT_LOGIN_UNKNOWN_ERROR] = "    .";
    m_Messages[BILLING_RESULT_LOGIN_IP_COM_ERROR] = "IP  COM .";
    m_Messages[BILLING_RESULT_LOGIN_IP_ERROR] = " IP    .";
    m_Messages[BILLING_RESULT_LOGIN_KEY_COM_ERROR] = "   COM .";
    m_Messages[BILLING_RESULT_LOGIN_NO_KEY] = "  .";
}

string BillingResultLoginErrorMessage::getMessage(int index) const {
    HashMapMessageConstItr itr = m_Messages.find(index);

    if (itr == m_Messages.end()) {
        filelog("BillingResultLoginErrorMessage.txt", "No Message ID : %d", index);
        return "";
    }

    return itr->second;
}

#endif
