#include "Common.h"
#include "MemoryPoolV2.h"

bool MemoryChunkManagerV2::Initialize(UINT chunkNo, SIZE_T dataSize, SIZE_T maxCount)
{
	_ASSERT(chunkNo != UINT_MAX);
	_ASSERT(dataSize > 0);
	_ASSERT(maxCount > 0);

	m_BufferCount = maxCount;
	m_ChunkSize = HEADER_SIZE + dataSize;

	SIZE_T size = m_ChunkSize * maxCount;
	m_pBuffer = malloc(size);
	if (!m_pBuffer)
	{
		return false;
	}
	ZeroMemory(m_pBuffer, size);

	MemoryChunk* pBuffer = nullptr;
	BYTE* pBufferPtr = (BYTE*)m_pBuffer;
	for (SIZE_T i = 0; i < maxCount; ++i)
	{
		pBuffer = (MemoryChunk*)(pBufferPtr + m_ChunkSize * i);

		pBuffer->TotalSize = m_ChunkSize;
		pBuffer->AccessSize = dataSize;
		pBuffer->ChunkManagerNo = chunkNo;
		pBuffer->AllocState = FALSE;

		LinkElemIntoListFIFO(&m_pFreeListHead, &m_pFreeListTail, &pBuffer->SortLink);
	}

	return true;
}

bool MemoryChunkManagerV2::Cleanup()
{
	while (m_pFreeListHead)
	{
		ListElem* pElem = m_pFreeListHead;
		UnLinkElemFromList(&m_pFreeListHead, &m_pFreeListTail, pElem);
	}
	if (m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = nullptr;
	}
	m_BufferCount = 0;
	m_ChunkSize = 0;

	return true;
}

void* MemoryChunkManagerV2::Alloc()
{
	if (!m_pFreeListHead)
	{
		return nullptr;
	}

	ListElem* pElem = m_pFreeListHead;
	UnLinkElemFromList(&m_pFreeListHead, &m_pFreeListTail, pElem);

	BYTE* pTemp = (BYTE*)pElem - sizeof(SIZE_T); // calc MemoryChunk ptr.
	MemoryChunk* pChunk = (MemoryChunk*)pTemp;
	pChunk->AllocState = TRUE;

	return pChunk->Memory;
}

void MemoryChunkManagerV2::Free(void* ptr)
{
	BYTE* pTemp = (BYTE*)ptr - HEADER_SIZE;
	MemoryChunk* pChunk = (MemoryChunk*)pTemp;
	pChunk->AllocState = FALSE;

	LinkElemIntoListFIFO(&m_pFreeListHead, &m_pFreeListTail, &pChunk->SortLink);
}

bool MemoryPoolV2::Initialize(SIZE_T maxCount)
{
	_ASSERT(maxCount > 0);

	const SIZE_T KB = 1024;
	const SIZE_T MB = 1024 * 1024;

	m_pBuffer0 = new MemoryChunkManagerV2;
	if (!m_pBuffer0 || !m_pBuffer0->Initialize(0, KB, maxCount))
	{
		return false;
	}

	m_pBuffer1 = new MemoryChunkManagerV2;
	if (!m_pBuffer1 || !m_pBuffer1->Initialize(1, 10 * KB, maxCount))
	{
		return false;
	}

	m_pBuffer2 = new MemoryChunkManagerV2;
	if (!m_pBuffer2 || !m_pBuffer2->Initialize(2, 100 * KB, maxCount))
	{
		return false;
	}

	m_pBuffer3 = new MemoryChunkManagerV2;
	if (!m_pBuffer3 || !m_pBuffer3->Initialize(3, MB, maxCount))
	{
		return false;
	}

	m_pBuffer4 = new MemoryChunkManagerV2;
	if (!m_pBuffer4 || !m_pBuffer4->Initialize(4, 10 * MB, maxCount))
	{
		return false;
	}

	return true;
}

bool MemoryPoolV2::Cleanup()
{
	if (m_pBuffer0)
	{
		delete m_pBuffer0;
		m_pBuffer0 = nullptr;
	}
	if (m_pBuffer1)
	{
		delete m_pBuffer1;
		m_pBuffer1 = nullptr;
	}
	if (m_pBuffer2)
	{
		delete m_pBuffer2;
		m_pBuffer2 = nullptr;
	}
	if (m_pBuffer3)
	{
		delete m_pBuffer3;
		m_pBuffer3 = nullptr;
	}
	if (m_pBuffer4)
	{
		delete m_pBuffer4;
		m_pBuffer4 = nullptr;
	}

	return true;
}

void* MemoryPoolV2::Alloc(SIZE_T size)
{
	if (size == 0)
	{
		return nullptr;
	}


	const SIZE_T KB = 1024;
	const SIZE_T MB = 1024 * 1024;

	if (size <= KB)
	{
		return m_pBuffer0->Alloc();
	}
	if (size <= 10 * KB)
	{
		return m_pBuffer1->Alloc();
	}
	if (size <= 100 * KB)
	{
		return m_pBuffer2->Alloc();
	}
	if (size <= MB)
	{
		return m_pBuffer3->Alloc();
	}
	if (size <= 10 * MB)
	{
		return m_pBuffer4->Alloc();
	}

	return nullptr;
}

void MemoryPoolV2::Free(void* ptr)
{
	_ASSERT(ptr);

	BYTE* pTemp = (BYTE*)ptr;
	MemoryChunk* pChunk = (MemoryChunk*)(pTemp - MemoryChunkManagerV2::HEADER_SIZE);

	if (pChunk->AccessBoundary != 0)
	{
		__debugbreak();
	}

	switch (pChunk->ChunkManagerNo)
	{
		case 0:
			m_pBuffer0->Free(ptr);
			break;

		case 1:
			m_pBuffer1->Free(ptr);
			break;

		case 2:
			m_pBuffer2->Free(ptr);
			break;

		case 3:
			m_pBuffer3->Free(ptr);
			break;

		case 4:
			m_pBuffer4->Free(ptr);
			break;

		default:
			__debugbreak();
	}
}
