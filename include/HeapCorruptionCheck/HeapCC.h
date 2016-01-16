#ifndef HEAPCC_H
#define HEAPCC_H

#if defined(_DEBUG) && defined(_WIN32) && !(defined(WINAPI_FAMILY) && ((WINAPI_FAMILY==WINAPI_FAMILY_APP) || (WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP)))
/** Enable the check heap corruption only for Win3|Debug platform (NOT WinPhone or WinStore)*/
#define CHECK_HEAP_CORRUPTION
#endif

#if defined(CHECK_HEAP_CORRUPTION)
    #include <atomic>
    #include <new>
    #include <exception>
    #ifdef _MSC_VER
    #pragma warning( disable : 4290 ) // C++ exception specification ignored.
    #endif
    void* operator new (std::size_t size, bool dummyBool);
    void* operator new[](std::size_t size, bool dummyBool);
    void operator delete (void* p, bool dummyBool) throw();
    void operator delete[](void* p, bool dummyBool) throw();
    /** If you want to use the global overloaded operators use the NEW_HCC define*/
    #define NEW_HCC new (true)
    #ifdef _MSC_VER
    #pragma warning( default : 4290 )
    #endif
#else //!CHECK_HEAP_CORRUPTION
    #define NEW_HCC new (true)
#endif //CHECK_HEAP_CORRUPTION

namespace hcc
{
#if defined(CHECK_HEAP_CORRUPTION)
	class HeapCC
	{
	public:

		void* operator new(size_t size);
		void operator delete(void* p);

		void* operator new[](size_t size);
		void operator delete[](void* p);


		void* operator new(size_t size, const char *file, int line);
		void operator delete(void* p, const char *file, int line);

		void* operator new[](size_t size, const char *file, int line);
		void operator delete[](void* p, const char *file, int line);

		/**
		* Allocate the memory. This is a static function because must be called from
        * new operators.
		* @param nSize an unsigned long representing the size of needed memory.
		* @param bAlignTop 
		* @return the address of an allocated memory chunk.*/
		static void* Alloc(size_t nSize, bool bAlignTop = true);

		/**
		* Free memory unit. This is a static function because must be called from
        * delete operators.
		* @param pPtr a pointer to the memory that must be freed*/
		static void Free(void* pPtr);

	protected:

		/** The constructor. */
        HeapCC(){}

		/** The destructor. */
        virtual ~HeapCC(){}

	private:

        /** This is the page size*/
		static std::atomic<unsigned long> s_dwPageSize;
	};
#else //!CHECK_HEAP_CORRUPTION
    class HeapCC {};
#endif //CHECK_HEAP_CORRUPTION
} // namespace hcc

#endif //HEAPCC_H
