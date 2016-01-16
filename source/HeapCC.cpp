#include "HeapCorruptionCheck/HeapCC.h"

#if defined(CHECK_HEAP_CORRUPTION)

#include <windows.h>
#include <stdio.h>
#include <iostream>


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




#ifdef _MSC_VER
#pragma warning( disable : 4290 ) // C++ exception specification ignored.
#endif
void* operator new (std::size_t size, bool checkHeapCorruption)
{
	return hcc::HeapCC::Alloc(size);
}
void* operator new[](std::size_t size, bool checkHeapCorruption)
{
	return hcc::HeapCC::Alloc(size);
}

void operator delete (void* p, bool checkHeapCorruption) throw()
{
	hcc::HeapCC::Free(p);
}
void operator delete[](void* p, bool checkHeapCorruption) throw()
{
	hcc::HeapCC::Free(p);
}
#ifdef _MSC_VER
#pragma warning( default : 4290 )
#endif


namespace hcc
{  
    std::atomic<unsigned long> HeapCC::s_dwPageSize = 0;

	CHAR* GetLastErrorText(CHAR *pBuf, ULONG bufSize);

	void* HeapCC::Alloc(size_t nSize, bool bAlignTop)
	{
		if (s_dwPageSize == 0)
		{
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			s_dwPageSize = sysInfo.dwPageSize;
		}

		PBYTE pPtr = (PBYTE)VirtualAlloc(nullptr, nSize + s_dwPageSize, MEM_RESERVE, PAGE_NOACCESS);
		if (!pPtr)
		{
			CHAR msgText[256];
			GetLastErrorText(msgText,sizeof(msgText));
			std::cout << "ERROR:" << msgText << std::endl;
			return nullptr;
		}

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
		{
			CHAR msgText[256];
			GetLastErrorText(msgText, sizeof(msgText));
			std::cout << "ERROR:" << msgText << std::endl;
			return nullptr;
		}

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

		VERIFY(VirtualFree(pPtr, mbi.RegionSize, MEM_DECOMMIT));
	}


	/** converts "Lasr Error" code into text
	* @param pBuff the buffer
	* @param bufSize the size of the buffer
	* @return error message*/
	CHAR* GetLastErrorText(CHAR *pBuf, ULONG bufSize)
	{
		DWORD retSize;
		LPTSTR pTemp = NULL;

		if (bufSize < 16) 
		{
			if (bufSize > 0) 
			{
				pBuf[0] = '\0';
			}
			return(pBuf);
		}
		retSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_ARGUMENT_ARRAY,
			NULL,
			GetLastError(),
			LANG_NEUTRAL,
			(LPTSTR)&pTemp,
			0,
			NULL);
		if (!retSize || pTemp == NULL) 
		{
			pBuf[0] = '\0';
		}
		else 
		{
			pTemp[strlen(pTemp) - 2] = '\0'; //remove cr and newline character
			sprintf(pBuf, "%0.*s (0x%x)", bufSize - 16, pTemp, GetLastError());
			LocalFree((HLOCAL)pTemp);
		}
		return(pBuf);
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

} // namespace hcc

#endif //CHECK_HEAP_CORRUPTION

