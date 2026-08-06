 

#pragma		once

class MemoryPool
{
public :
	MemoryPool( int BlockSize, int BlockCount );
	~MemoryPool();

	void*					Alloc();
	void					Free( void *pMem );

	bool					IsPtrInPool( void *pMem );				
	bool					IsAvailablePtr( void *pMem );			

private :
	class CBlock
	{
	public :
		CBlock				*m_pPrev;
		int					m_leftBlocks;
		unsigned char		*m_pNextBlock;
	};

	class CFreeBlock												
	{
	public :
		CFreeBlock			*m_pPrev;
	};

	CBlock					*m_pCurrentBlock;
	CFreeBlock				*m_pFreeBlockList;

	int						m_BlockSize;
	int						m_BlockCount;

};

extern MemoryPool g_CreatureMemoryPool;
extern MemoryPool g_CreatureWearMemoryPool;
extern MemoryPool g_NPCCreatureMemoryPool;
extern MemoryPool g_FakeCreatureMemoryPool;