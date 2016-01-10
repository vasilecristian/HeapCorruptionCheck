# HeapCorruptionCheck
This is a library that uses VirtualAlloc & VirtualFree to detect access to a already used memory page. Is inspired by http://www.codeproject.com/Articles/38340/Immediate-memory-corruption-detection .
This works only for Windows and does not detects all type of heap corruption.

## Why I need it.
1. Because I need to check legacy code.  
2. Because I need it in my tests.
 
This is still somewhat a work in progress; it's stable, but there are probably places where can be improved.

## Licensing 
Public Domain. If my licensing is wrong, please let me know. Use at your own risk for whatever you want. Apparently licensing is hard and complicated. If your country doesn't have a public domain, feel free to say you found this on the side of the road. 


## Overview
`HeapCorruptionCheck` is a simple class that can be derived or not. To use it only on all instances of a class, just make that class to extend the hcc::HeapCC class . If you want to use it on all allocations of your app, just overload the new and delete operators.

Example: 
```c++

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
	int* tt = (int*)&aClass->m_a + 4;
	*tt = 100; /// CRASH:  Access violation writing location 
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


	
	/*char str[16];
	str[0] = '\0';
	for (int i = 0; i<argc; i++) 
	{
		strcat(str, argv[i]);    /// CRASH: Stack around the variable 'str' was corrupted.
		if (i < (argc - 1)) 
			strcat(str, " ");
	}*/


	return 0;
}
```
