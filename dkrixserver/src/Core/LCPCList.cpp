//----------------------------------------------------------------------
//
// Filename    : LCPCList.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LCPCList.h"

#include "PCOustersInfo.h"
#include "PCSlayerInfo.h"
#include "PCVampireInfo.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
LCPCList::LCPCList()

    : m_Agree(1) {
    for (uint i = 0; i < SLOT_MAX; i++)
        m_pPCInfos[i] = NULL;
}


//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
LCPCList::~LCPCList()

{
    
    for (uint i = 0; i < SLOT_MAX; i++) {
        SAFE_DELETE(m_pPCInfos[i]);
    }
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void LCPCList::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    //--------------------------------------------------
    
    //
    // *OPTMIZATION*
    //
    
    //
    //--------------------------------------------------
    char pcTypes[SLOT_MAX];

    for (uint i = 0; i < SLOT_MAX; i++)
        iStream.read(pcTypes[i]);

    //--------------------------------------------------
    
    //--------------------------------------------------
    for (uint j = 0; j < SLOT_MAX; j++) {
        switch (pcTypes[j]) {
        case 'S': {
            PCSlayerInfo* pPCSlayerInfo = new PCSlayerInfo();
            pPCSlayerInfo->read(iStream);
            m_pPCInfos[pPCSlayerInfo->getSlot()] = pPCSlayerInfo;
        } break;

        case 'V': {
            PCVampireInfo* pPCVampireInfo = new PCVampireInfo();
            pPCVampireInfo->read(iStream);
            m_pPCInfos[pPCVampireInfo->getSlot()] = pPCVampireInfo;
        } break;

        case 'O': {
            PCOustersInfo* pPCOustersInfo = new PCOustersInfo();
            pPCOustersInfo->read(iStream);
            m_pPCInfos[pPCOustersInfo->getSlot()] = pPCOustersInfo;
        } break;

        case '0':
            break;

        default:
            throw InvalidProtocolException("invalid pc type");
        }
    }

#ifdef __NETMARBLE_SERVER__
    iStream.read(m_Agree);
#endif

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LCPCList::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    //--------------------------------------------------
    
    //
    
    //
    // ex>
    // 	S0V : Slayer-EMPTY-VAMPIRE
    // 	00S : EMPTY-EMPTY-SLAYER
    //
    //--------------------------------------------------
    for (uint i = 0; i < SLOT_MAX; i++) {
        if (m_pPCInfos[i]) { // m_pPCInfos[i] != NULL

            if (m_pPCInfos[i]->getPCType() == PC_SLAYER) {
                oStream.write('S');
            } else if (m_pPCInfos[i]->getPCType() == PC_VAMPIRE) { // case of PC_VAMPIRE
                oStream.write('V');
            } else {
                oStream.write('O');
            }

        } else { // m_pPCInfos[i] == NULL
            oStream.write('0');
        }
    }

    //--------------------------------------------------
    
    //--------------------------------------------------
    for (uint j = 0; j < SLOT_MAX; j++) {
        if (m_pPCInfos[j] != NULL) {
            m_pPCInfos[j]->write(oStream);
        }
    }

#ifdef __NETMARBLE_SERVER__
    oStream.write(m_Agree);
#endif

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void LCPCList::execute(Player* pPlayer)

{
    __BEGIN_TRY

    LCPCListHandler::execute(this, pPlayer);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
PacketSize_t LCPCList::getPacketSize() const

{
    PacketSize_t packetSize = 0;

    // write() always emits one slot type byte per character slot before the
    // variable-size PC payloads. The old size omitted these bytes, which left
    // the client packet stream misaligned after the character list.
    packetSize = SLOT_MAX * sizeof(char);

    for (uint i = 0; i < SLOT_MAX; i++) {
        if (m_pPCInfos[i]) { // m_pPCInfos[i] != NULL
            packetSize += m_pPCInfos[i]->getSize();
        }
    }

#ifdef __NETMARBLE_SERVER__
    packetSize += szBYTE;
#endif

    return packetSize;
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string LCPCList::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "LCPCList(\n";

    for (uint i = 0; i < SLOT_MAX; i++)
        if (m_pPCInfos[i] != NULL)
            msg << m_pPCInfos[i]->toString() << "\n";
        else
            msg << "EMPTY SLOT\n";

    msg << ")";

    return msg.toString();

    __END_CATCH
}
