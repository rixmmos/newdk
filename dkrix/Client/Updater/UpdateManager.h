//--------------------------------------------------------------------------------
// 
// Filename    : UpdateManager.h 
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

#ifndef __UPDATE_MANAGER_H__
#define __UPDATE_MANAGER_H__

#pragma warning(disable:4786)

// include files
#include "Update.h"
#include <list>

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
const uint maxUpdates = 10240;

//--------------------------------------------------------------------------------
//
// class UpdateManager
//

//

//
// load/save
// read(SocketInputStream&)/write(SocketOutputStream&)
// read(Socket*)/write(Socket*)
//

//
//--------------------------------------------------------------------------------

class UpdateManager{

public :

	// constructor
	UpdateManager ();

	// destructor
	~UpdateManager ();
	

public :

	// load from update file
	void load ( const std::string & filename ) ;

	// save to update file
	void save ( const std::string & filename ) const ;

	// get size
	uint getSize () const ;

	// get file size
	uint getFileSize () const ;

	// get max size
	static uint getMaxSize ()  { return Update::getMaxSize() * maxUpdates; }


public :

	// list methods
	void push_back ( Update * pUpdate )  { m_Updates.push_back(pUpdate); }
	void pop_front () { m_Updates.pop_front(); }
	Update * front () const { return m_Updates.front(); }
	bool empty () const { return m_Updates.empty(); }
	int	getNum() const	{ return m_Updates.size(); }

	// get debug std::string
	std::string toString () const throw ();

	void		operator = (const UpdateManager& um);
	void		operator += (const UpdateManager& um);


private :

	// list of Update
	std::list< Update * > m_Updates;

};

#endif
