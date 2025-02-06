#pragma once

class MemoryChunkManager;

class MemoryPool
{
public:
	MemoryPool() = default;
	~MemoryPool() { Cleanup(); }

	bool Initialize(UINT maxCount);
	bool Cleanup();

	void* Alloc(UINT size);
	void Free(void* ptr);

private:
	BYTE* m_pBuffer = nullptr;
	UINT m_BufferSize = 0;

	MemoryChunkManager* m_pBuffer0 = nullptr; // 1KB chunk.
	MemoryChunkManager* m_pBuffer1 = nullptr; // 10KB chunk.
	MemoryChunkManager* m_pBuffer2 = nullptr; // 100KB chunk.
	MemoryChunkManager* m_pBuffer3 = nullptr; // 1MB chunk.
	MemoryChunkManager* m_pBuffer4 = nullptr; // 10MB chunk.
	//MemoryChunkManager* m_pBuffer5 = nullptr; // 100MB chunk.
	//MemoryChunkManager* m_pBuffer6 = nullptr; // 1GB chunk.

};

