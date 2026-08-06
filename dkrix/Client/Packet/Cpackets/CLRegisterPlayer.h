//////////////////////////////////////////////////////////////////////
// 
// Filename    : CLRegisterPlayer.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CL_REGISTER_PLAYER_H__
#define __CL_REGISTER_PLAYER_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "../PlayerInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class CLRegisterPlayer;
//


//
//////////////////////////////////////////////////////////////////////

class CLRegisterPlayer : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CL_REGISTER_PLAYER; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw ()
	{
		
		return    szBYTE + m_ID.size() 			
				+ szBYTE + m_Password.size() 	
				+ szBYTE + m_Name.size() 		
				+ szBYTE						
				+ szBYTE + m_SSN.size() 		
				+ szBYTE + m_Telephone.size() 	
				+ szBYTE + m_Cellular.size() 	
				+ szBYTE + m_ZipCode.size() 	
				+ szBYTE + m_Address.size() 	
				+ szBYTE 						
				+ szBYTE + m_Email.size() 		
				+ szBYTE + m_Homepage.size() 	
				+ szBYTE + m_Profile.size() 	
				+ szBYTE;						
	}

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName () const throw () { return "CLRegisterPlayer"; }
	
	// get packet's debug string
	std::string toString () const throw ();
#endif

public :

	//----------------------------------------------------------------------
	// *CAUTION* 
	
	
	//----------------------------------------------------------------------

    // get/set player's id
	std::string getID () const throw () { return m_ID; }
	void setID ( std::string id ) throw () { m_ID = ( id.size() > PlayerInfo::maxIDLength ) ? id.substr(0,PlayerInfo::maxIDLength) : id; }

    // get/set player's password
    std::string getPassword () const throw () { return m_Password; }
    void setPassword ( std::string password ) throw () { m_Password = ( password.size() > PlayerInfo::maxPasswordLength ) ? password.substr(0,PlayerInfo::maxPasswordLength) : password; }

    // get/set player's name
    std::string getName () const throw () { return m_Name; }
    void setName ( std::string name ) throw () { m_Name = ( name.size() > PlayerInfo::maxNameLength ) ? name.substr(0,PlayerInfo::maxNameLength) : name; }

    // get/set player's sex
    Sex getSex () const throw () { return m_Sex; }
    void setSex ( Sex sex ) throw () { m_Sex = sex; }

    // get/set player's ssn
    std::string getSSN () const throw () { return m_SSN; }
    void setSSN ( std::string ssn ) throw () { m_SSN = ( ssn.size() > PlayerInfo::maxSSNLength ) ? ssn.substr(0,PlayerInfo::maxSSNLength) : ssn; }

    // get/set player's telephone
    std::string getTelephone () const throw () { return m_Telephone; }
    void setTelephone ( std::string telephone ) throw () { m_Telephone = ( telephone.size() > PlayerInfo::maxTelephoneLength ) ? telephone.substr(0,PlayerInfo::maxTelephoneLength) : telephone; }

    // get/set player's cellular
    std::string getCellular () const throw () { return m_Cellular; }
    void setCellular ( std::string cellular ) throw () { m_Cellular = ( cellular.size() > PlayerInfo::maxCellularLength ) ? cellular.substr(0,PlayerInfo::maxCellularLength) : cellular; }

    // get/set player's zipcode
    std::string getZipCode () const throw () { return m_ZipCode; }
    void setZipCode ( std::string zipcode ) throw () { m_ZipCode = ( zipcode.size() > PlayerInfo::maxZipCodeLength ) ? zipcode.substr(0,PlayerInfo::maxZipCodeLength) : zipcode; }

    // get/set player's address
    std::string getAddress () const throw () { return m_Address; }
    void setAddress ( std::string address ) throw () { m_Address = ( address.size() > PlayerInfo::maxAddressLength ) ? address.substr(0,PlayerInfo::maxAddressLength) : address; }

    // get/set player's nation
    Nation getNation () const throw () { return m_Nation; }
    void setNation ( Nation nation ) throw () { m_Nation = nation; }

    // get/set player's email
    std::string getEmail () const throw () { return m_Email; }
    void setEmail ( std::string email ) throw () { m_Email = ( email.size() > PlayerInfo::maxEmailLength ) ? email.substr(0,PlayerInfo::maxEmailLength) : email; }

    // get/set player's homepage
    std::string getHomepage () const throw () { return m_Homepage; }
    void setHomepage ( std::string homepage ) throw () { m_Homepage = ( homepage.size() > PlayerInfo::maxHomepageLength ) ? homepage.substr(0,PlayerInfo::maxHomepageLength) : homepage; }

    // get/set player's profile
    std::string getProfile () const throw () { return m_Profile; }
    void setProfile ( std::string profile ) throw () { m_Profile = ( profile.size() > PlayerInfo::maxProfileLength ) ? profile.substr(0,PlayerInfo::maxProfileLength) : profile; }

	// get/set player info's publicability (?) 
	bool getPublic () const throw () { return m_bPublic; }
	void setPublic ( bool bPublic ) throw () { m_bPublic = bPublic; }

private :

	//--------------------------------------------------
	
	//--------------------------------------------------
    std::string m_ID; 			
    std::string m_Password; 		
	//--------------------------------------------------
	
	//--------------------------------------------------
    std::string m_Name; 			
    Sex m_Sex; 				
    std::string m_SSN; 			
	//--------------------------------------------------
	
	//--------------------------------------------------
    std::string m_Telephone; 	
    std::string m_Cellular; 		
    std::string m_ZipCode; 		
    std::string m_Address; 		
    Nation m_Nation; 		
	//--------------------------------------------------
	
	//--------------------------------------------------
    std::string m_Email; 		
    std::string m_Homepage; 		
	//--------------------------------------------------
	
	//--------------------------------------------------
    std::string m_Profile; 		
	bool m_bPublic; 		

};


//////////////////////////////////////////////////////////////////////
//
// class CLRegisterPlayerFactory;
//
// Factory for CLRegisterPlayer
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CLRegisterPlayerFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CLRegisterPlayer(); }

	// get packet name
	std::string getPacketName () const throw () { return "CLRegisterPlayer"; }

	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_REGISTER_PLAYER; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw ()
	{
		
		return    szBYTE + PlayerInfo::maxIDLength 			
				+ szBYTE + PlayerInfo::maxPasswordLength 	
				+ szBYTE + PlayerInfo::maxNameLength 		
				+ szBYTE									
				+ szBYTE + PlayerInfo::maxSSNLength 		
				+ szBYTE + PlayerInfo::maxTelephoneLength 	
				+ szBYTE + PlayerInfo::maxCellularLength 	
				+ szBYTE + PlayerInfo::maxZipCodeLength 	
				+ szBYTE + PlayerInfo::maxAddressLength 	
				+ szBYTE 									
				+ szBYTE + PlayerInfo::maxEmailLength 		
				+ szBYTE + PlayerInfo::maxHomepageLength 	
				+ szBYTE + PlayerInfo::maxProfileLength 	
				+ szBYTE;									
	}


};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CLRegisterPlayerHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CLRegisterPlayerHandler {

	public :

		// execute packet's handler
		static void execute ( CLRegisterPlayer * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
