//----------------------------------------------------------------------
//
// Filename    : PlayerInfo.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __PLAYER_INFO_H__
#define __PLAYER_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"

//----------------------------------------------------------------------
//
// class PlayerInfo;
//

//
//----------------------------------------------------------------------
namespace PlayerInfo {

	
	const uint minIDLength = 4;
	const uint maxIDLength = 10;

	
	const uint minPasswordLength = 6;
	const uint maxPasswordLength = 10;

	
	const uint maxNameLength = 20;

	
	const uint maxSSNLength = 20;

	
	const uint maxTelephoneLength = 15;

	
	const uint maxCellularLength = 15;

	
	const uint maxZipCodeLength = 7;

	
	const uint maxAddressLength = 100;

	
	const uint maxEmailLength = 50;

	
	const uint maxHomepageLength = 50;

	
	const uint maxProfileLength = 200;

};

#endif
