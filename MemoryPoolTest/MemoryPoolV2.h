#pragma once

#include "LinkedList.h"

struct MemoryChunk
{
	// Header.
	SIZE_T AccessBoundary; // could be 0.
	ListElem SortLink;
	SIZE_T TotalSize; // total chunk size.
	SIZE_T AccessSize; // real memory part size.
	UINT ChunkManagerNo; // chunk manager number.
	UINT AllocState;

	// Body.
	BYTE Memory[1];
};

class MemoryChunkManagerV2
{
public:
	static const SIZE_T HEADER_SIZE = sizeof(SIZE_T) * 3 + sizeof(ListElem) + sizeof(UINT) * 2;

public:
	MemoryChunkManagerV2() = default;
	~MemoryChunkManagerV2() { Cleanup(); }

	bool Initialize(UINT chunkNo, SIZE_T dataSize, SIZE_T maxCount);
	bool Cleanup();

	void* Alloc();
	void Free(void* ptr);

private:
	void* m_pBuffer = nullptr;
	SIZE_T m_BufferCount = 0;
	SIZE_T m_ChunkSize = 0;

	ListElem m_FreeList = { 0, };
	ListElem* m_pFreeListHead = nullptr;
	ListElem* m_pFreeListTail = nullptr;
};

class MemoryPoolV2
{
public:
	MemoryPoolV2() = default;
	~MemoryPoolV2() { Cleanup(); }

	bool Initialize(SIZE_T maxCount);
	bool Cleanup();

	void* Alloc(SIZE_T size);
	void Free(void* ptr);

private:
	MemoryChunkManagerV2* m_pBuffer0 = nullptr; // 1kb
	MemoryChunkManagerV2* m_pBuffer1 = nullptr; // 10kb
	MemoryChunkManagerV2* m_pBuffer2 = nullptr; // 100kb
	MemoryChunkManagerV2* m_pBuffer3 = nullptr; // 1mb
	MemoryChunkManagerV2* m_pBuffer4 = nullptr; // 10mb
};
