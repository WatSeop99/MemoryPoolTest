// MemoryPoolTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
#include <Windows.h>
#include "MemoryPool.h"
#include "MemoryPoolV2.h"
#include "WinMemoryPool.h"


int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    MemoryPool pool;
    if (!pool.Initialize(100))
    {
        __debugbreak();
    }

    MemoryPoolV2 pool2;
    if (!pool2.Initialize(100))
    {
        __debugbreak();
    }

    LARGE_INTEGER initCounter;
    LARGE_INTEGER startCounter;
    LARGE_INTEGER curCounter;
    float elapsedSec;
    UINT64 elapsedCounter;

    const SIZE_T KB = 1024;
    const SIZE_T MB = 1024 * 1024;
    const SIZE_T MAX_TEST_COUNT = 100000000;

    srand((UINT)time(nullptr));

    std::cout << "Start test!\n";

    QueryPerformanceFrequency(&initCounter);

    QueryPerformanceCounter(&startCounter);

    for (SIZE_T i = 0; i < MAX_TEST_COUNT; ++i)
    {
        UINT bytes = rand() % (100 * MB) + KB;
        void* pBuffer = malloc(bytes);
        free(pBuffer);
    }

    QueryPerformanceCounter(&curCounter);

    elapsedCounter = curCounter.QuadPart - startCounter.QuadPart;
    elapsedSec = (float)elapsedCounter / (float)initCounter.QuadPart;
    std::cout << "normal malloc-free elapsed " << elapsedSec << " sec.\n";

    QueryPerformanceCounter(&startCounter);

    for (SIZE_T i = 0; i < MAX_TEST_COUNT; ++i)
    {
        UINT bytes = rand() % (100 * MB) + KB;
        void* pBuffer = pool.Alloc(bytes);
        pool.Free(pBuffer);
    }

    QueryPerformanceCounter(&curCounter);

    elapsedCounter = curCounter.QuadPart - startCounter.QuadPart;
    elapsedSec = (float)elapsedCounter / (float)initCounter.QuadPart;
    std::cout << "custom pool(MemoryPool) elapsed " << elapsedSec << " sec.\n";


    HANDLE m_hHeap = HeapCreate(0, 0, 100 * 1024 * 1024);

    QueryPerformanceCounter(&startCounter);

    for (SIZE_T i = 0; i < MAX_TEST_COUNT; ++i)
    {
        UINT bytes = rand() % (100 * MB) + KB;
        void* pBuffer = HeapAlloc(m_hHeap, 0, bytes);
        _ASSERT(HeapFree(m_hHeap, 0, pBuffer));
    }

    QueryPerformanceCounter(&curCounter);

    elapsedCounter = curCounter.QuadPart - startCounter.QuadPart;
    elapsedSec = (float)elapsedCounter / (float)initCounter.QuadPart;
    std::cout << "custom pool2(windows heap) elapsed " << elapsedSec << " sec.\n";

    QueryPerformanceCounter(&startCounter);

    for (SIZE_T i = 0; i < MAX_TEST_COUNT; ++i)
    {
        UINT bytes = rand() % (100 * MB) + KB;
        void* pBuffer = pool2.Alloc(bytes);
        pool2.Free(pBuffer);
    }

    QueryPerformanceCounter(&curCounter);

    elapsedCounter = curCounter.QuadPart - startCounter.QuadPart;
    elapsedSec = (float)elapsedCounter / (float)initCounter.QuadPart;
    std::cout << "custom pool3(MemoryPoolV2) elapsed " << elapsedSec << " sec.\n";

    HeapDestroy(m_hHeap);

    pool.Cleanup();
    pool2.Cleanup();

#ifdef _DEBUG
    _ASSERT(_CrtCheckMemory());
#endif

    return 0;
}
