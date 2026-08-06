//------------------------------------------------------------------------
// CToken.h
//------------------------------------------------------------------------

//													Coding : 1998.11.
//													Modify : 1998.11.20
//------------------------------------------------------------------------
#ifndef	__CTOKEN_H__
#define	__CTOKEN_H__

#include <string.h>


class CToken {
	public :
		// constructor/destructor
		CToken(const char* str=NULL);
		~CToken();

		// set string
		void SetString(const char* str);

		
		void SkipSpace();


		
		const char* GetToken(const char* delimiter=" ");

		
		const char* GetEnd();



	//------------------------------------
	// private member functions
	private :

		
		void Release();



	//------------------------------------
	// data members
	protected :

		
		char* m_pString;

		
		char* m_pCurrent;
};

#endif


