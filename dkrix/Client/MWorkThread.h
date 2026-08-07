//----------------------------------------------------------------------
// MWorkThread.h
//----------------------------------------------------------------------
//
// [ Init ]
//
//   = Init(LPTHREAD_START_ROUTINE FileThreadProc, int priority);
//


//


//
//		long
//		LoadingThreadProc(void* pParam)
//		{
//			g_pLoadingThread->Execute();
//
//			return 0L;
//		}
//
//
// [ Remove ]
//


//
//
// [ AddFirst / AddLast ]
//





//
//
// [ SetPriority ]
//


//
//		THREAD_PRIORITY_HIGHEST
//		THREAD_PRIORITY_ABOVE_NORMAL
//		THREAD_PRIORITY_NORMAL
//		THREAD_PRIORITY_BELOW_NORMAL
//		THREAD_PRIORITY_LOWEST
//
//----------------------------------------------------------------------
#ifndef __MWORKTHREAD_H__
#define	__MWORKTHREAD_H__

#include "../basic/Platform.h"

#include <mutex>

// Type definitions for thread function pointer (Windows API compatibility)
typedef DWORD (*LPTHREAD_START_ROUTINE)(void* lpParameter);

// Thread priority constants (Windows API compatibility)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_ABOVE_NORMAL    1
#define THREAD_PRIORITY_BELOW_NORMAL   -1
#define THREAD_PRIORITY_HIGHEST          2
#define THREAD_PRIORITY_LOWEST          -2

// Wait constants (Windows API compatibility)
#define WAIT_OBJECT_0                   0
#define WAIT_TIMEOUT                    258

// Stub for WaitForSingleObject - maps to event wait
static inline DWORD WaitForSingleObject(HANDLE event, DWORD timeout) {
    platform_event_t evt = (platform_event_t)event;
    if (platform_event_wait(evt, timeout) == 0) {
        return WAIT_OBJECT_0;
    }
    return WAIT_TIMEOUT;
}

// Stub for SetThreadPriority (not implemented on mingw/macOS)
static inline BOOL SetThreadPriority(HANDLE thread, int priority) {
    (void)thread; (void)priority;
    return TRUE;
}

#include <deque>
#include "MWorkNode.h"

//----------------------------------------------------------------------
// WorkThread
//----------------------------------------------------------------------
class MWorkThread {
	public :
		typedef std::deque<MWorkNode*>	WORKNODE_DEQUE;

	public :
		MWorkThread();
		~MWorkThread();

		//---------------------------------------------------
		// Init / Release
		//---------------------------------------------------
		void				Init(LPTHREAD_START_ROUTINE FileThreadProc, int priority=THREAD_PRIORITY_NORMAL);
      	void				Release();
		void				ReleaseWork();

		//---------------------------------------------------
		// Execute
		//---------------------------------------------------
		void				Execute();

		//---------------------------------------------------
		// Get Size
		//---------------------------------------------------
		int					GetSize() const				{ return m_dequeWorkNode.size(); }

		//---------------------------------------------------
		// Is...
		//---------------------------------------------------
		BOOL				IsWorking() const			{ return WaitForSingleObject(m_hHasWorkEvent, 0) == WAIT_OBJECT_0; }
		BOOL				IsFinishCurrentWork() const	{ return WaitForSingleObject(m_hEndWorkEvent, 0) == WAIT_OBJECT_0; }
		BOOL				IsStopWork() const			{ return WaitForSingleObject(m_hStopWorkEvent, 0) == WAIT_OBJECT_0; }
		
		//---------------------------------------------------
		// Stop
		//---------------------------------------------------
		void				Remove(int type);

		//---------------------------------------------------
		// Add work
		//---------------------------------------------------
		void				AddFirst(MWorkNode* pNode);
		void				AddLast(MWorkNode* pNode);

		//---------------------------------------------------
		// Set Priority
		//---------------------------------------------------
		void				SetPriority(int priority)	{ SetThreadPriority(m_hWorkThread, priority); }
			
	protected :
		//---------------------------------------------------
		// Lock Deque
		//---------------------------------------------------
		 
		void				LockDeque()					{ m_csDeque.lock(); }
		void				UnlockDeque()				{ m_csDeque.unlock(); }

		std::mutex	m_csDeque;					


		//---------------------------------------------------
		// Lock Current
		//---------------------------------------------------
		 
		void				LockCurrent()					{ m_csCurrent.lock(); }
		void				UnlockCurrent()					{ m_csCurrent.unlock(); }

		std::mutex	m_csCurrent;					

	protected :
		HANDLE				m_hWorkThread;
		HANDLE				m_hHasWorkEvent;	
		HANDLE				m_hEndWorkEvent;	
		HANDLE				m_hStopWorkEvent;	
		
		WORKNODE_DEQUE		m_dequeWorkNode;

		
		MWorkNode*			m_pCurrentWork;
};

#endif


