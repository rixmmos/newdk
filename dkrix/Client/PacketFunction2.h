//-----------------------------------------------------------------------------
// PacketFunction2.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#ifndef __PACKETFUNCTION2_H__
#define __PACKETFUNCTION2_H__

class MActionResultNode;
class MCreature;

extern MActionResultNode*	CreateActionResultNode(MCreature* pCreature, int skillID, BYTE grade = 0);

#endif