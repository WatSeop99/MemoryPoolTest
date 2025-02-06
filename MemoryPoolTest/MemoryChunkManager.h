#pragma once

#include "LinkedList.h"

class HashTable;

class MemoryChunkManager
{
public:
	MemoryChunkManager() = default;
	~MemoryChunkManager() { Cleanup(); }

	bool Initialize(SIZE_T dataSize, SIZE_T maxCount);
	bool Cleanup();

	void* Alloc();
	void Free(void* ptr);

private:
	BYTE* m_pBuffer = nullptr;
	SIZE_T m_BufferCount = 0;
	SIZE_T m_MaxBufferCount = 0;
	SIZE_T m_DataSize = 0;

	ListElem m_Freelist = { 0, };
	HashTable* m_pAllocList = nullptr;
};
