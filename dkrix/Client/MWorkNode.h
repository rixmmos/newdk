//----------------------------------------------------------------------
// MWorkNode.h
//----------------------------------------------------------------------

//


// 
// [Execute]




//



//



//
//----------------------------------------------------------------------

#ifndef __MWORKNODE_H__
#define	__MWORKNODE_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif

//----------------------------------------------------------------------
// WorkNode
//----------------------------------------------------------------------
class MWorkNode {
	public :
		MWorkNode();		
		virtual ~MWorkNode();

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		void				SetType(int type)	{ m_Type = type; }
		BOOL				IsTypeOf(int type)	{ return m_Type==type; }
		int					GetType() const		{ return m_Type; }

		//--------------------------------------------------------
		// Stop
		//--------------------------------------------------------
		BOOL				IsExecute() const	{ return m_bExecute; }
		BOOL				IsStop() const		{ return !m_bExecute; }
		void				Stop()				{ m_bExecute = FALSE; }

		//--------------------------------------------------------
		// Execute
		//--------------------------------------------------------
		virtual BOOL		Execute(MWorkNode*& pNode) = 0;

	protected :
		
		int					m_Type;

		
		BOOL				m_bExecute;

		
		HANDLE				m_hStopEvent;
};

#endif


