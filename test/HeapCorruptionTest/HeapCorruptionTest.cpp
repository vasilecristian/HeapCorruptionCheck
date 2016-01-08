// MemPoolTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <array>
#include "HeapCorruptionCheck/HeapCC.h"


using namespace std;


void* operator new (size_t nSize)
{
	return hcc::HeapCC::Alloc(nSize, true);
}
void operator delete (void* pPtr)
{
	hcc::HeapCC::Free(pPtr);
}



void* operator new[] (size_t nSize)
{
	return hcc::HeapCC::Alloc(nSize, true);
}
void operator delete[] (void* pPtr)
{
	hcc::HeapCC::Free(pPtr);
}


class A : public hcc::HeapCC
{
public:
	int m_a;
	int m_b;
	int m_c;
	int m_d;
};





int main(int argc, char* argv[])
{
	A haha;
	A* aClass = new A();
	A* bClass = new A();
	A* cClass = aClass + 1;
	//cClass->m_a = 0; /// CRASH:  Access violation writing location 

	int* a = new int(2);
	int* b = new int(4);
	int* c = a;
	*c = 5;
	//*(b+1) = 6;/// CRASH:  Access violation writing location 
	

	std::array<A*, 100> MyArray;
	MyArray[0] = new A();
	//MyArray[111] = new A(); /// CRASH: array subscript out of bounds


	A* MyArray2[100];
	for (int i = 0; i < 100; i++)
	{
		MyArray2[i] = new A();
	}
	//MyArray2[100] = new A(); /// CRASH: Stack around the variable 'MyArray2' was corrupted.


	
	char str[16];
	str[0] = '\0';
	for (int i = 0; i<argc; i++) 
	{
		strcat(str, argv[i]);    /// CRASH: Stack around the variable 'str' was corrupted.
		if (i < (argc - 1)) 
			strcat(str, " ");
	}


	return 0;
}
