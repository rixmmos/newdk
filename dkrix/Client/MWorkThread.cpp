//----------------------------------------------------------------------
// MWorkThread.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MWorkThread.h"

#if defined(PLATFORM_WINDOWS) && !defined(_M_X64)
// Windows implementation (original code)

//----------------------------------------------------------------------
// Platform-specific wrappers for Windows API compatibility
static inline BOOL SetEvent(HANDLE event) {
    return platform_event_signal((platform_event_t)event) == 0 ? TRUE : FALSE;
}

static inline BOOL ResetEvent(HANDLE event) {
    return platform_event_reset((platform_event_t)event) == 0 ? TRUE : FALSE;
}

#include <process.h>

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MWorkThread::MWorkThread()
{
	m_hWorkThread = 0;
	m_hHasWorkEvent = 0;
	m_hEndWorkEvent = 0;
	m_hStopWorkEvent = 0;
	m_pCurrentWork = NULL;
}

MWorkThread::~MWorkThread()
{
	Release();
}

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void MWorkThread::Init(LPTHREAD_START_ROUTINE FileThreadProc, int priority)
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	m_hHasWorkEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
	m_hEndWorkEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
	m_hStopWorkEvent = CreateEvent( NULL, FALSE, FALSE, NULL);

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	m_hWorkThread = (HANDLE)platform_thread_create(
		FileThreadProc, 
		this,
		priority
	);

	if(m_hWorkThread==0)
	{
		Release();
		return;
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void MWorkThread::Release()
{
	if(m_hWorkThread)
	{
		
		SetEvent( m_hStopWorkEvent );

		
		platform_thread_wait( m_hWorkThread );
		platform_thread_close( m_hWorkThread );
		m_hWorkThread = 0;
	}

	if(m_hHasWorkEvent)
	{
		platform_event_close( m_hHasWorkEvent );
		m_hHasWorkEvent = 0;
	}

	if(m_hEndWorkEvent)
	{
		platform_event_close( m_hEndWorkEvent );
		m_hEndWorkEvent = 0;
	}

	if(m_hStopWorkEvent)
	{
		platform_event_close( m_hStopWorkEvent );
		m_hStopWorkEvent = 0;
	}

	//--------------------------------------------------------------------
	// List
	//--------------------------------------------------------------------
	ReleaseWork();
}

//----------------------------------------------------------------------
// ReleaseWork
//----------------------------------------------------------------------
void MWorkThread::ReleaseWork()
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	MWorkNode* pNode;
	while((pNode = GetFirstWorkNode())!=NULL)
	{
		Remove( pNode );
	}

	m_pCurrentWork = NULL;
}

//----------------------------------------------------------------------
// Execute
//----------------------------------------------------------------------
void MWorkThread::Execute()
{
	while(1)
	{
		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		WaitForSingleObject(m_hHasWorkEvent, PLATFORM_INFINITE);

		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		if(WaitForSingleObject(m_hStopWorkEvent, 0)==WAIT_OBJECT_0)
		{
			break;
		}

		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		if(m_pCurrentWork)
		{
			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			m_pCurrentWork->Execute();

			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			SetEvent( m_hEndWorkEvent );
		}
	}
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------
void MWorkThread::Remove(int type)
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	MWorkNode* pNode;
	MWorkNode* pPrevNode = NULL;

	for(pNode=m_pFirstWorkNode; pNode; pNode=pNode->m_pNext)
	{
		if(pNode->GetType()==type)
		{
			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			if(pNode==m_pCurrentWork)
			{
				
				WaitForSingleObject( m_hEndWorkEvent, PLATFORM_INFINITE );
			}

			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			Remove( pNode, pPrevNode );
			pPrevNode = NULL; 
		}
		else
		{
			pPrevNode = pNode;
		}
	}
}

//----------------------------------------------------------------------
// AddFirst
//----------------------------------------------------------------------
void MWorkThread::AddFirst(MWorkNode* pNode)
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	if(m_pFirstWorkNode==NULL)
	{
		m_pFirstWorkNode = pNode;
		m_pLastWorkNode = pNode;
	}
	else
	{
		pNode->m_pNext = m_pFirstWorkNode;
		m_pFirstWorkNode->m_pPrev = pNode;
		m_pFirstWorkNode = pNode;
	}

	pNode->m_pWorkThread = this;
}

//----------------------------------------------------------------------
// AddLast
//----------------------------------------------------------------------
void MWorkThread::AddLast(MWorkNode* pNode)
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	if(m_pFirstWorkNode==NULL)
	{
		m_pFirstWorkNode = pNode;
		m_pLastWorkNode = pNode;
	}
	else
	{
		pNode->m_pPrev = m_pLastWorkNode;
		m_pLastWorkNode->m_pNext = pNode;
		m_pLastWorkNode = pNode;
	}

	pNode->m_pWorkThread = this;
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------
void MWorkThread::Remove(MWorkNode* pNode, MWorkNode* pPrevNode)
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	if(pPrevNode)
	{
		pPrevNode->m_pNext = pNode->m_pNext;
	}
	else
	{
		m_pFirstWorkNode = pNode->m_pNext;
	}

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	if(pNode->m_pNext)
	{
		pNode->m_pNext->m_pPrev = pPrevNode;
	}
	else
	{
		m_pLastWorkNode = pPrevNode;
	}

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	delete pNode;
}

//----------------------------------------------------------------------
// GetFirstWorkNode
//----------------------------------------------------------------------
MWorkNode* MWorkThread::GetFirstWorkNode()
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	MWorkNode* pNode = m_pFirstWorkNode;
	if(pNode==NULL)
	{
		return NULL;
	}

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	m_pFirstWorkNode = pNode->m_pNext;
	if(m_pFirstWorkNode)
	{
		m_pFirstWorkNode->m_pPrev = NULL;
	}
	else
	{
		m_pLastWorkNode = NULL;
	}

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	m_pCurrentWork = pNode;

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	pNode->m_pPrev = NULL;
	pNode->m_pNext = NULL;

	return pNode;
}

//----------------------------------------------------------------------
// GetFirstWorkNode (with type)
//----------------------------------------------------------------------
MWorkNode* MWorkThread::GetFirstWorkNode(int type)
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	MWorkNode* pNode = m_pFirstWorkNode;
	MWorkNode* pPrevNode = NULL;

	while(pNode)
	{
		if(pNode->GetType()==type)
		{
			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			if(pPrevNode)
			{
				pPrevNode->m_pNext = pNode->m_pNext;
			}
			else
			{
				m_pFirstWorkNode = pNode->m_pNext;
			}

			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			if(pNode->m_pNext)
			{
				pNode->m_pNext->m_pPrev = pPrevNode;
			}
			else
			{
				m_pLastWorkNode = pPrevNode;
			}

			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			m_pCurrentWork = pNode;

			//--------------------------------------------------------------------
			
			//--------------------------------------------------------------------
			pNode->m_pPrev = NULL;
			pNode->m_pNext = NULL;

			return pNode;
		}

		pPrevNode = pNode;
		pNode = pNode->m_pNext;
	}

	return NULL;
}

//----------------------------------------------------------------------
// ExecuteWorkNode
//----------------------------------------------------------------------
void MWorkThread::ExecuteWorkNode()
{
	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	m_pCurrentWork = GetFirstWorkNode();
	if(m_pCurrentWork==NULL)
	{
		return;
	}

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	SetEvent( m_hHasWorkEvent );
}

#else
// Non-Windows platforms (macOS/Linux) - Stub implementations

#include "MWorkThread.h"

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MWorkThread::MWorkThread()
{
	m_hWorkThread = 0;
	m_hHasWorkEvent = 0;
	m_hEndWorkEvent = 0;
	m_hStopWorkEvent = 0;
	m_pCurrentWork = NULL;
}

MWorkThread::~MWorkThread()
{
	// Stub implementation
}

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void MWorkThread::Init(LPTHREAD_START_ROUTINE FileThreadProc, int priority)
{
	// Stub implementation
	(void)FileThreadProc;
	(void)priority;
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void MWorkThread::Release()
{
	// Stub implementation
}

//----------------------------------------------------------------------
// ReleaseWork
//----------------------------------------------------------------------
void MWorkThread::ReleaseWork()
{
	// Stub implementation
}

//----------------------------------------------------------------------
// Execute
//----------------------------------------------------------------------
void MWorkThread::Execute()
{
	// Stub implementation - not functional on non-Windows
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------
void MWorkThread::Remove(int type)
{
	(void)type;
}

//----------------------------------------------------------------------
// AddFirst
//----------------------------------------------------------------------
void MWorkThread::AddFirst(MWorkNode* pNode)
{
	(void)pNode;
}

//----------------------------------------------------------------------
// AddLast
//----------------------------------------------------------------------
void MWorkThread::AddLast(MWorkNode* pNode)
{
	(void)pNode;
}

#endif /* PLATFORM_WINDOWS && !_M_X64 */
