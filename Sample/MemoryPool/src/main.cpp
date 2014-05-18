/******************
main.cpp
******************/

/*!\file main.cpp
 * \brief Test-Program Main (Entry Point for the application).
 */

#include "BasicIncludes.h"
#include "MemPool/CMemoryPool.h"

#include <windows.h>

MemPool::CMemoryPool *g_ptrMemPool = NULL  ; //!< Global MemoryPool (Testing purpose)
unsigned int TestCount             = 50000 ; //!< Nr of (de-)allocations (Testing purpose)
unsigned int ArraySize             = 1000  ; //!< Size of the "Testing"-Array

/*! \class MyTestClass_OPOverload
 *  \brief Test Class (Operator new/delete overloaded)
 *
 * The only purpose of this class is the testing of the Memory-Pool.
 */
class MyTestClass_OPOverload
{
public :
	MyTestClass_OPOverload()
	{
	  m_cMyArray[0] = 'H' ;
	  m_cMyArray[1] = 'e' ;
	  m_cMyArray[2] = 'l' ;
	  m_cMyArray[3] = 'l' ;
	  m_cMyArray[4] = 'o' ;
	  m_cMyArray[5] = NULL ;
	  m_strMyString = "This is a small Test-String" ;
	  m_iMyInt = 12345 ;

	  m_fFloatValue = 23456.7890f ;
      m_fDoubleValue = 6789.012345 ;

      Next = this ;
	}

	virtual ~MyTestClass_OPOverload() {} ;

    void *operator new(std::size_t ObjectSize)
	{
		return g_ptrMemPool->GetMemory(ObjectSize) ;
	}

	void operator delete(void *ptrObject, std::size_t ObjectSize)
	{
		g_ptrMemPool->FreeMemory(ptrObject, ObjectSize) ;
	}
private :
   // Test-Data
   char m_cMyArray[25] ;
   unsigned char m_BigArray[10000] ;
   std::string m_strMyString ;
   int m_iMyInt ;
   MyTestClass_OPOverload *Next ;
   float m_fFloatValue ;
   double m_fDoubleValue ;
} ;

/*! \class MyTestClass
 *  \brief Test Class ("Original" new/delete operator)
 *
 * The only purpose of this class is the testing of the Memory-Pool.
 */
class MyTestClass
{
public :
	MyTestClass()
	{
	  m_cMyArray[0] = 'H' ;
	  m_cMyArray[1] = 'e' ;
	  m_cMyArray[2] = 'l' ;
	  m_cMyArray[3] = 'l' ;
	  m_cMyArray[4] = 'o' ;
	  m_cMyArray[5] = NULL ;
	  m_strMyString = "This is a small Test-String" ;
	  m_iMyInt = 12345 ;

	  m_fFloatValue = 23456.7890f ;
      m_fDoubleValue = 6789.012345 ;

      Next = this ;
	}

	virtual ~MyTestClass() {} ;
private :
   // Test-Data
   char m_cMyArray[25] ;
   unsigned char m_BigArray[10000] ;
   std::string m_strMyString ;
   int m_iMyInt ;
   MyTestClass *Next ;
   float m_fFloatValue ;
   double m_fDoubleValue ;
} ;

/******************
CreateGlobalMemPool
******************/
void CreateGlobalMemPool()
{
  std::cerr << "Creating MemoryPool...." ;
  g_ptrMemPool = new MemPool::CMemoryPool() ;
  std::cerr << "OK" << std::endl ;
}

/******************
DestroyGlobalMemPool
******************/
void DestroyGlobalMemPool()
{
  std::cerr << "Deleting MemPool...." ;
  if(g_ptrMemPool) delete g_ptrMemPool ;
  std::cerr << "OK" << std::endl ;
}

/******************
TestAllocationSpeedClassMemPool
******************/
void TestAllocationSpeedClassMemPool()
{
  std::cerr << "Allocating Memory (Object Size : " << sizeof(MyTestClass_OPOverload) << ")..." ;
  timeBeginPeriod(1) ;
  unsigned int MyStartTimeB = timeGetTime() ;
  for(unsigned int j = 0; j < TestCount; j++)
  {
	MyTestClass_OPOverload *ptrTestClass = new MyTestClass_OPOverload ;
	delete ptrTestClass ;
  }
  unsigned int MyEndTimeB = timeGetTime() ;
  timeEndPeriod(1) ;
  std::cerr << "OK" << std::endl ;

  std::cerr << "Result for MemPool(Class Test) : " << (MyEndTimeB - MyStartTimeB) << " ms" << std::endl ;
}

/******************
TestAllocationSpeedClassHeap
******************/
void TestAllocationSpeedClassHeap()
{
  std::cerr << "Allocating Memory (Object Size : " << sizeof(MyTestClass) << ")..." ;
  timeBeginPeriod(1) ;
  unsigned int MyStartTimeB = timeGetTime() ;
  for(unsigned int j = 0; j < TestCount; j++)
  {
	MyTestClass *ptrTestClass = new MyTestClass ;
    delete ptrTestClass ;
  }
  unsigned int MyEndTimeB = timeGetTime() ;
  timeEndPeriod(1) ;
  std::cerr << "OK" << std::endl ;

  std::cerr << "Result for Heap(Class Test)    : " << (MyEndTimeB - MyStartTimeB) << " ms" << std::endl ;
}

/******************
TestAllocationSpeedArrayMemPool
******************/
void TestAllocationSpeedArrayMemPool()
{
  std::cerr << "Allocating Memory (Object Size : " << ArraySize << ")..." ;
  timeBeginPeriod(1) ;
  unsigned int MyStartTimeB = timeGetTime() ;
  for(unsigned int j = 0; j < TestCount; j++)
  {
	  char *ptrArray = (char *) g_ptrMemPool->GetMemory(ArraySize)  ;
	  g_ptrMemPool->FreeMemory(ptrArray, ArraySize) ;
  }
  unsigned int MyEndTimeB = timeGetTime() ;
  timeEndPeriod(1) ;
  std::cerr << "OK" << std::endl ;

  std::cerr << "Result for MemPool(Array-Test) : " << (MyEndTimeB - MyStartTimeB) << " ms" << std::endl ;
}

/******************
TestAllocationSpeedArrayHeap
******************/
void TestAllocationSpeedArrayHeap()
{
  std::cerr << "Allocating Memory (Object Size : " << ArraySize << ")..." ;
  timeBeginPeriod(1) ;
  unsigned int MyStartTimeB = timeGetTime() ;
  for(unsigned int j = 0; j < TestCount; j++)
  {
	 char *ptrArray = (char *) malloc(ArraySize)  ;
	 free(ptrArray) ;
  }
  unsigned int MyEndTimeB = timeGetTime() ;
  timeEndPeriod(1) ;
  std::cerr << "OK" << std::endl ;

  std::cerr << "Result for Heap(Array-Test)    : " << (MyEndTimeB - MyStartTimeB) << " ms" << std::endl ;
}



/******************
WriteMemoryDumpToFile
******************/
void WriteMemoryDumpToFile()
{
  std::cerr << "Writing MemoryDump to File..." ;
  g_ptrMemPool->WriteMemoryDumpToFile("MemoryDump.bin") ;
  std::cerr << "OK" << std::endl ;
}

/******************
main
******************/
int main(int argc, char *argv[])
{
	std::cout << "MemoryPool Program started..." << std::endl ;
	CreateGlobalMemPool() ;

	TestAllocationSpeedArrayMemPool() ;
	TestAllocationSpeedArrayHeap() ;

	TestAllocationSpeedClassMemPool() ;
	TestAllocationSpeedClassHeap() ;

	//WriteMemoryDumpToFile() ;

	DestroyGlobalMemPool() ;
	std::cout << "MemoryPool Program finished..." << std::endl ;
	system("PAUSE") ;
	return 0 ;
}
