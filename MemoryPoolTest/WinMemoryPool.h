#pragma once

class HashTable;

class WinMemoryPool
{
public:
	WinMemoryPool() = default;
	~WinMemoryPool() { Cleanup(); }

	bool Initialize();
	bool Cleanup();

	void* Alloc(SIZE_T size);
	void Free(void* ptr);

private:
	HANDLE m_hHeap0 = nullptr;
	HANDLE m_hHeap1 = nullptr;
	HANDLE m_hHeap2 = nullptr;
	HANDLE m_hHeap3 = nullptr;
	HANDLE m_hHeap4 = nullptr;

	HashTable* m_pHashTable = nullptr;
};
