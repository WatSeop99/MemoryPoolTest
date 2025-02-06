#include "Common.h"
#include "HashTable.h"

void HashTable::Initialize(UINT maxBucketNum, UINT maxKeySize)
{
	_ASSERT(maxBucketNum > 0);
	_ASSERT(maxKeySize > 0);

	m_MaxBucketNum = maxBucketNum;
	m_MaxKeyDataSize = maxKeySize;

	m_pBucketTable = new Bucket[maxBucketNum];
	ZeroMemory(m_pBucketTable, sizeof(Bucket) * maxBucketNum);

	srand((UINT)time(nullptr));
}

UINT HashTable::Select(void** ppItemList, UINT maxItemNum, const void* pKeyData, UINT size)
{
	_ASSERT(ppItemList);
	_ASSERT(pKeyData);

	UINT selectedItemNum = 0;
	UINT index = CreateKey(pKeyData, size, m_MaxBucketNum);
	Bucket* pBucket = m_pBucketTable + index;

	ListElem* pCurBucket = pBucket->pBucketLinkHead;
	BucketItem* pBucketItem;
	SIZE_T keyVal = (SIZE_T)pKeyData;

	while (pCurBucket)
	{
		if (!maxItemNum)
		{
			break;
		}
		if (selectedItemNum >= maxItemNum)
		{
			break;
		}

		pBucketItem = (BucketItem*)pCurBucket->pItem;

		if (pBucketItem->Size != size)
		{
			goto LB_NEXT;
		}
		if (memcmp(pBucketItem->pKeyData, &keyVal, size))
		{
			goto LB_NEXT;
		}

		--maxItemNum;

		ppItemList[selectedItemNum] = (void*)pBucketItem->pItem;
		++selectedItemNum;

	LB_NEXT:
		pCurBucket = pCurBucket->pNext;
	}

	return selectedItemNum;
}

void* HashTable::Insert(const void* pItem, const void* pKeyData, UINT size)
{
	_ASSERT(pItem);
	_ASSERT(pKeyData);

	void* pSearchHandle = nullptr;

	if (size > m_MaxKeyDataSize)
	{
		goto LB_RET;
	}

	UINT bucketMemSize = (UINT)(sizeof(BucketItem) - sizeof(BYTE)) + m_MaxKeyDataSize;
	BucketItem* pBucketItem = (BucketItem*)malloc(bucketMemSize);

	UINT index = CreateKey(pKeyData, size, m_MaxBucketNum);
	Bucket* pBucket = m_pBucketTable + index;

	pBucketItem->pItem = pItem;
	pBucketItem->Size = size;
	pBucketItem->pBucket = pBucket;
	pBucketItem->SortLink.pPrev = nullptr;
	pBucketItem->SortLink.pNext = nullptr;
	pBucketItem->SortLink.pItem = pBucketItem;
	pBucket->LinkNum++;

	SIZE_T keyData = (SIZE_T)pKeyData;
	memcpy(pBucketItem->pKeyData, &keyData, size);

	LinkElemIntoListFIFO(&pBucket->pBucketLinkHead, &pBucket->pBucketLinkTail, &pBucketItem->SortLink);

	++m_ItemNum;
	pSearchHandle = pBucketItem;

LB_RET:
	return pSearchHandle;
}

void HashTable::Delete(const void* pKeyData, bool bIsBucketData)
{
	_ASSERT(pKeyData);

	if (bIsBucketData)
	{
		BucketItem* pBucketItem = (BucketItem*)pKeyData;
		Bucket* pBucket = pBucketItem->pBucket;

		UnLinkElemFromList(&pBucket->pBucketLinkHead, &pBucket->pBucketLinkTail, &pBucketItem->SortLink);
		--(pBucket->LinkNum);

		free(pBucketItem);
		--m_ItemNum;

		return;
	}

	SIZE_T index = CreateKey(pKeyData, sizeof(void*), m_MaxBucketNum);
	Bucket* pBucket = m_pBucketTable + index;
	if (!pBucket->pBucketLinkHead)
	{
		return;
	}

	ListElem* pCurBucket = pBucket->pBucketLinkHead;
	SIZE_T keyVal = (SIZE_T)pKeyData;
	while (pCurBucket)
	{
		BucketItem* pBucketItem = (BucketItem*)pCurBucket->pItem;
		if (memcmp(pBucketItem->pKeyData, &keyVal, pBucketItem->Size) == 0)
		{
			UnLinkElemFromList(&pBucket->pBucketLinkHead, &pBucket->pBucketLinkTail, &pBucketItem->SortLink);
			free(pBucketItem);
			--m_ItemNum;
			break;
		}

		pCurBucket = pCurBucket->pNext;
	}
}

void HashTable::DeleteAll()
{
	BucketItem* pBucketItem;
	for (UINT i = 0; i < m_MaxBucketNum; ++i)
	{
		while (m_pBucketTable[i].pBucketLinkHead)
		{
			pBucketItem = (BucketItem*)m_pBucketTable[i].pBucketLinkHead->pItem;
			Delete(pBucketItem);
		}
	}
}

void HashTable::Cleanup()
{
	ResourceCheck();

	DeleteAll();
	if (m_pBucketTable)
	{
		delete[] m_pBucketTable;
		m_pBucketTable = nullptr;
	}
}

UINT HashTable::CreateKey(const void* pData, UINT size, UINT bucketNum)
{
	UINT keyData = 0;
	SIZE_T entry = (SIZE_T)pData;
	while (size > 0)
	{
		keyData += (UINT)(entry & size);
		size /= 2;
	}

	UINT index = keyData % bucketNum;
	return index;
}

void HashTable::ResourceCheck()
{
	if (m_ItemNum)
	{
		__debugbreak();
	}
}

UINT HashFuncInt(void* pKey, SIZE_T size, SIZE_T maxBucketNum)
{
	UINT keyData = 0;
	SIZE_T entry = (SIZE_T)pKey;
	while (size > 0)
	{
		keyData += (UINT)(entry & size);
		size /= 2;
	}

	UINT index = keyData % maxBucketNum;
	return index;
}
