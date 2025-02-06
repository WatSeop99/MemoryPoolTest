#include "Common.h"
#include "MemoryChunkManager.h"
#include "MemoryPool.h"

bool MemoryPool::Initialize(UINT maxCount)
{
	_ASSERT(maxCount > 0);

	bool bRet = false;

	const SIZE_T KB = 1024;
	const SIZE_T MB = 1024 * 1024;

	m_pBuffer0 = new MemoryChunkManager;
	m_pBuffer0->Initialize(KB, maxCount);
	m_pBuffer1 = new MemoryChunkManager;
	m_pBuffer1->Initialize(10 * KB, maxCount);
	m_pBuffer2 = new MemoryChunkManager;
	m_pBuffer2->Initialize(100 * KB, maxCount);
	m_pBuffer3 = new MemoryChunkManager;
	m_pBuffer3->Initialize(MB, maxCount);
	m_pBuffer4 = new MemoryChunkManager;
	m_pBuffer4->Initialize(10 * MB, maxCount);

	bRet = true;

LB_RET:
	return bRet;
}

bool MemoryPool::Cleanup()
{
	if (m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = nullptr;
	}
	m_BufferSize = 0;

	if (m_pBuffer4)
	{
		delete m_pBuffer4;
		m_pBuffer4 = nullptr;
	}
	if (m_pBuffer3)
	{
		delete m_pBuffer3;
		m_pBuffer3 = nullptr;
	}
	if (m_pBuffer2)
	{
		delete m_pBuffer2;
		m_pBuffer2 = nullptr;
	}
	if (m_pBuffer1)
	{
		delete m_pBuffer1;
		m_pBuffer1 = nullptr;
	}
	if (m_pBuffer0)
	{
		delete m_pBuffer0;
		m_pBuffer0 = nullptr;
	}

	return true;
}

void* MemoryPool::Alloc(UINT size)
{
	const SIZE_T KB = 1024;
	const SIZE_T MB = 1024 * 1024;

	/*if (size == 0 || size > 10 * MB)
	{
		return nullptr;
	}

	if (size < 10 * KB)
	{
		return m_pBuffer0->Alloc();
	}
	if (size >= 10 * KB && size < 100 * KB)
	{
		return m_pBuffer1->Alloc();
	}
	if (size >= 100 * KB && size < MB)
	{
		return m_pBuffer2->Alloc();
	}
	if (size >= MB && size < 10 * MB)
	{
		return m_pBuffer3->Alloc();
	}
	if (size == 10 * MB)
	{
		return m_pBuffer4->Alloc();
	}*/

	return m_pBuffer4->Alloc();

	return nullptr;
}

void MemoryPool::Free(void* ptr)
{
	if (!ptr)
	{
		return;
	}

	//m_pBuffer0->Free(ptr);
	//m_pBuffer1->Free(ptr);
	//m_pBuffer2->Free(ptr);
	//m_pBuffer3->Free(ptr);
	m_pBuffer4->Free(ptr);
}
