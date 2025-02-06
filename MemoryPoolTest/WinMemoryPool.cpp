#include "Common.h"
#include "HashTable.h"
#include "WinMemoryPool.h"

bool WinMemoryPool::Initialize()
{
	const SIZE_T KB = 1024;
	const SIZE_T MB = 1024 * 1024;

	m_hHeap0 = HeapCreate(0, KB, 100 * KB);
	if (!m_hHeap0 || m_hHeap0 == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_hHeap1 = HeapCreate(0, 10 * KB, 100 * 10 * KB);
	if (!m_hHeap1 || m_hHeap1 == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_hHeap2 = HeapCreate(0, 100 * KB, 100 * 100 * KB);
	if (!m_hHeap2 || m_hHeap2 == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_hHeap3 = HeapCreate(0, MB, 100 * MB);
	if (!m_hHeap3 || m_hHeap3 == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_hHeap4 = HeapCreate(0, 10 * MB, 100 * 10 * MB);
	if (!m_hHeap4 || m_hHeap4 == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_pHashTable = new HashTable;
	m_pHashTable->Initialize(100, sizeof(void*));

	return true;
}

bool WinMemoryPool::Cleanup()
{
	if (m_hHeap4)
	{
		HeapDestroy(m_hHeap4);
		m_hHeap4 = nullptr;
	}
	if (m_hHeap3)
	{
		HeapDestroy(m_hHeap3);
		m_hHeap3 = nullptr;
	}
	if (m_hHeap2)
	{
		HeapDestroy(m_hHeap2);
		m_hHeap2 = nullptr;
	}
	if (m_hHeap1)
	{
		HeapDestroy(m_hHeap1);
		m_hHeap1 = nullptr;
	}
	if (m_hHeap0)
	{
		HeapDestroy(m_hHeap0);
		m_hHeap0 = nullptr;
	}

	if (m_pHashTable)
	{
		delete m_pHashTable;
		m_pHashTable = nullptr;
	}

	return true;
}

void* WinMemoryPool::Alloc(SIZE_T size)
{
	const SIZE_T KB = 1024;
	const SIZE_T MB = 1024 * 1024;

	if (size == 0 || size > 10 * MB)
	{
		return nullptr;
	}

	void* pRet = nullptr;
	int* pHeapIndex = new int;

	if (size < 10 * KB)
	{
		*pHeapIndex = 0;
		pRet = HeapAlloc(m_hHeap0, HEAP_ZERO_MEMORY, size);
	}
	if (size < 100 * KB)
	{
		*pHeapIndex = 1;
		pRet = HeapAlloc(m_hHeap1, HEAP_ZERO_MEMORY, size);
	}
	if (size < MB)
	{
		*pHeapIndex = 2;
		pRet = HeapAlloc(m_hHeap2, HEAP_ZERO_MEMORY, size);
	}
	if (size < 10 * MB)
	{
		*pHeapIndex = 3;
		pRet = HeapAlloc(m_hHeap3, HEAP_ZERO_MEMORY, size);
	}
	if (size < 100 * MB)
	{
		*pHeapIndex = 4;
		pRet = HeapAlloc(m_hHeap4, HEAP_ZERO_MEMORY, size);
	}

	if (pRet)
	{
		m_pHashTable->Insert(pHeapIndex, pRet, sizeof(void*));
	}

	return pRet;
}

void WinMemoryPool::Free(void* ptr)
{
	if (!ptr)
	{
		return;
	}

	int* pHeapIndex = nullptr;
	m_pHashTable->Select((void**)&pHeapIndex, 1, ptr, sizeof(void*));
	m_pHashTable->Delete(ptr, false);

	switch (*pHeapIndex)
	{
		case 0:
			HeapFree(m_hHeap0, 0, ptr);
			break;

		case 1:
			HeapFree(m_hHeap1, 0, ptr);
			break;

		case 2:
			HeapFree(m_hHeap2, 0, ptr);
			break;

		case 3:
			HeapFree(m_hHeap3, 0, ptr);
			break;

		case 4:
			HeapFree(m_hHeap4, 0, ptr);
			break;

		default:
			break;
	}

	delete pHeapIndex;
}
