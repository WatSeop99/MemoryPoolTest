#pragma once

#include "LinkedList.h"

typedef UINT(*PFN_HASH_GEN_FUNC)(void*, SIZE_T, SIZE_T);

UINT HashFuncInt(void* pKey, SIZE_T size, SIZE_T maxBucketNum);

struct Bucket
{
	ListElem* pBucketLinkHead;
	ListElem* pBucketLinkTail;
	UINT LinkNum;
};
struct BucketItem
{
	const void* pItem;
	Bucket* pBucket;
	ListElem SortLink;
	UINT Size;
	BYTE pKeyData[1];
};
class HashTable
{
public:
	HashTable() = default;
	~HashTable() { Cleanup(); };

	void Initialize(UINT maxBucketNum, UINT maxKeySize);

	UINT Select(void** ppItemList, UINT maxItemNum, const void* pKeyData, UINT size);
	void* Insert(const void* pItem, const void* pKeyData, UINT size);
	void Delete(const void* pKeyData, bool bIsBucketData = true);
	void DeleteAll();

	void Cleanup();

protected:
	UINT CreateKey(const void* pData, UINT size, UINT bucketNum);

	void ResourceCheck();

private:
	Bucket* m_pBucketTable = nullptr;
	UINT m_MaxBucketNum = 0;
	UINT m_MaxKeyDataSize = 0;
	UINT m_ItemNum = 0;
	PFN_HASH_GEN_FUNC m_pfnHashFunc = nullptr;
};
