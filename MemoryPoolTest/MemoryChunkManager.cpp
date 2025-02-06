#include "Common.h"
#include "HashTable.h"
#include "MemoryChunkManager.h"

bool MemoryChunkManager::Initialize(SIZE_T dataSize, SIZE_T maxCount)
{
	_ASSERT(dataSize > 0);
	_ASSERT(maxCount > 0);

	m_DataSize = dataSize;
	m_MaxBufferCount = maxCount;
	m_BufferCount = 0;

	m_pBuffer = new BYTE[maxCount];
	if (!m_pBuffer)
	{
		return false;
	}

	m_pAllocList = new HashTable;
	if (!m_pAllocList)
	{
		return false;
	}
	m_pAllocList->Initialize(maxCount, sizeof(void*));

	for (SIZE_T i = 0; i < maxCount; ++i)
	{
		ListElem* pElem = new ListElem;
		ZeroMemory(pElem, sizeof(ListElem));

		pElem->pItem = m_pBuffer + dataSize * i;
		LinkElemIntoListFIFO(&m_Freelist.pPrev, &m_Freelist.pNext, pElem);
	}

	return true;
}

bool MemoryChunkManager::Cleanup()
{
	if (m_pAllocList)
	{
		delete m_pAllocList;
		m_pAllocList = nullptr;
	}
	while (m_Freelist.pPrev)
	{
		ListElem* pElem = m_Freelist.pPrev;
		UnLinkElemFromList(&m_Freelist.pPrev, &m_Freelist.pNext, pElem);
		delete pElem;
	}

	if (m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = nullptr;
	}
	m_BufferCount = 0;
	m_MaxBufferCount = 0;
	m_DataSize = 0;

	return true;
}

void* MemoryChunkManager::Alloc()
{
	if (m_BufferCount + 1 >= m_MaxBufferCount)
	{
		//__debugbreak();
		return nullptr;
	}

	ListElem* pElem = m_Freelist.pPrev;
	UnLinkElemFromList(&m_Freelist.pPrev, &m_Freelist.pNext, pElem);

	if (!m_pAllocList->Insert(pElem, pElem->pItem, sizeof(void*)))
	{
		LinkElemIntoListFIFO(&m_Freelist.pPrev, &m_Freelist.pNext, pElem);
		//__debugbreak();
		return nullptr;
	}

	++m_BufferCount;

	return pElem->pItem;
}

void MemoryChunkManager::Free(void* ptr)
{
	_ASSERT(ptr);

	ListElem* pElem = nullptr;
	SIZE_T searchNum = m_pAllocList->Select((void**)&pElem, 1, ptr, sizeof(void*));
	if (searchNum == 0)
	{
		//__debugbreak();
		return;
	}

	m_pAllocList->Delete(ptr, false);
	LinkElemIntoListFIFO(&m_Freelist.pPrev, &m_Freelist.pNext, pElem);
	--m_BufferCount;
}
