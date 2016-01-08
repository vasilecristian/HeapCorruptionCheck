/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef HEAPCC_H
#define HEAPCC_H


namespace hcc
{
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
		* Allocate memory unit
		*
		* @param nSize an unsigned long representing the size of needed memory.
		* @param bAlignTop 
		* @return the address of an allocated memory chunk.
		*/
		static void* Alloc(size_t nSize, bool bAlignTop = true);

		/**
		* Free memory unit
		*
		* @param pPtr a pointer to the memory that must be freed
		*/
		static void Free(void* pPtr);

	protected:

		/** The constructor. */
		HeapCC();

		/** The destructor. */
		virtual ~HeapCC();

	private:

		static unsigned long s_dwPageSize;

	};

} // namespace hcc

#endif //HEAPCC_H
