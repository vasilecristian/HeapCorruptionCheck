#include "HeapCorruptionCheck/HeapCC.h"

#include <windows.h>


#ifdef _DEBUG
#	ifndef ASSERT
#		define ASSERT(x) do { if (!(x)) __debugbreak(); } while (false)
#	endif
#	ifndef VERIFY
#		define VERIFY(x) ASSERT(x)
#	endif
#else // _DEBUG
#	ifndef ASSERT
#		define ASSERT(x) do {} while (false)
#	endif
#	ifndef VERIFY
#		define VERIFY(x) (x)
#	endif
#endif // _DEBUG


namespace hcc
{  
	unsigned long HeapCC::s_dwPageSize = 0;

	HeapCC::HeapCC()
    {    
		
    }

	HeapCC::~HeapCC()
    {
    
    }


	void* HeapCC::Alloc(size_t nSize, bool bAlignTop)
	{
		if (s_dwPageSize == 0)
		{
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			s_dwPageSize = sysInfo.dwPageSize;
		}

		PBYTE pPtr = (PBYTE)VirtualAlloc(NULL, nSize + s_dwPageSize, MEM_RESERVE, PAGE_NOACCESS);
		if (!pPtr)
			return NULL;

		PBYTE pRet = pPtr;
		if (bAlignTop)
		{
			DWORD dwExtra = (DWORD)nSize % s_dwPageSize;
			if (dwExtra)
				pRet += s_dwPageSize - dwExtra;
		}
		else
			pRet += s_dwPageSize;

		if (!VirtualAlloc(pRet, nSize, MEM_COMMIT, PAGE_READWRITE))
			return NULL;

		// ok
		return pRet;

	}
    


	void HeapCC::Free(void* pPtr)
	{
		if (!pPtr)
			return;

		MEMORY_BASIC_INFORMATION mbi;
		VERIFY(VirtualQuery(pPtr, &mbi, sizeof(mbi)) == sizeof(mbi));

		ASSERT(MEM_COMMIT == mbi.State);
		ASSERT(PAGE_READWRITE == mbi.Protect);

		//size_t nReserveSize = mbi.RegionSize + s_dwPageSize;

		VERIFY(VirtualFree(pPtr, mbi.RegionSize, MEM_DECOMMIT));
	}



	void* HeapCC::operator new(size_t size)
    {
		void *p = HeapCC::Alloc(size);
    
		if (p == nullptr)
        {
            throw "allocation fail : no free memory";
        }
		return p;
	};

	void HeapCC::operator delete(void* p)
    {
		HeapCC::Free(p);
	};


	void* HeapCC::operator new[](size_t size)
	{
		void *p = HeapCC::Alloc(size);

		if (p == nullptr)
		{
			throw "allocation fail : no free memory";
		}
		return p;
	};

	void HeapCC::operator delete[](void* p)
	{
		HeapCC::Free(p);
	};



	void* HeapCC::operator new(size_t size, const char *file, int line)
    { 
		void *p = HeapCC::Alloc(size);

        return p;
    };

	void HeapCC::operator delete(void* p, const char *file, int line)
    { 
		HeapCC::Free(p);
    };



	void* HeapCC::operator new[](size_t size, const char *file, int line)
    { 
		void *p = HeapCC::Alloc(size);
        return p;
    }

	void HeapCC::operator delete[](void* p, const char *file, int line)
    { 
		HeapCC::Free(p);
    };

} // namespace gu

