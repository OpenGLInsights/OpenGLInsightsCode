//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <cassert>
#include <algorithm>

namespace glf
{
	//--------------------------------------------------------------------------
	template <typename T> 
	T* AlignedAllocate(int _count) 
	{
		return (T *)AlignedAllocate(_count * sizeof(T));
	}
	//--------------------------------------------------------------------------
	template <typename T>
	MemoryPool<T>::MemoryPool(int _countMax):
	count(0),
	countMax(_countMax),
	data(NULL)
	{
		data = AlignedAllocate<T>(countMax);
	}
	//--------------------------------------------------------------------------
	template <typename T>
	MemoryPool<T>::~MemoryPool() 
	{
		DesallocateAll();
		AlignedDesallocate(data);
		data = NULL;
	}
	//--------------------------------------------------------------------------
	template <typename T>
	T* MemoryPool<T>::Allocate() 
	{
		assert(count<countMax);
		T* ptr = new (&data[count]) T();
		++count;
		return ptr;
	}
	//--------------------------------------------------------------------------
	template <typename T>
	void MemoryPool<T>::DesallocateAll() 
	{
		for(int i=0;i<count;++i)
			data[i].~T();
		count = 0;
	}	
	//--------------------------------------------------------------------------
	template <class T>
	inline SmartPointer<T>::SmartPointer():
	object(0),
	objectCounter(0) 
	{

	}
	//-------------------------------------------------------------------------
	template <class T>
	inline SmartPointer<T>::SmartPointer(const SmartPointer<T>& copy) :
	object(copy.object),
	objectCounter(copy.objectCounter) 
	{
		if (objectCounter)
			++(*objectCounter);
	}	
	//--------------------------------------------------------------------------
	template <class T>
	inline SmartPointer<T>::SmartPointer(T* pointer) :
	object(pointer),
	objectCounter(new int(1))
	{

	}
	//--------------------------------------------------------------------------
	template <class T>
	inline SmartPointer<T>::~SmartPointer()
	{
		if (objectCounter && (--(*objectCounter) == 0))	
		{
			delete objectCounter;
			delete object;
		}
	}
	//--------------------------------------------------------------------------
	template <class T>
	inline void SmartPointer<T>::Swap(SmartPointer<T>& pointer)	
	{
		std::swap(object,pointer.object);
		std::swap(objectCounter, pointer.objectCounter);
	}
	//--------------------------------------------------------------------------
	template <class T>
	inline T& SmartPointer<T>::operator *() const	
	{
		assert(object != 0);
		return *object;
	}
	//--------------------------------------------------------------------------
	template <class T>
	inline T* SmartPointer<T>::operator ->() const 
	{
		assert(object != 0);
		return object;
	}
	//--------------------------------------------------------------------------
	template <class T>
	inline const SmartPointer<T>& SmartPointer<T>::operator =(const SmartPointer<T>& pointer)	
	{
		SmartPointer<T>(pointer).Swap(*this);
		return *this;
	}
	//--------------------------------------------------------------------------
	template <class T>
	inline SmartPointer<T>::operator T*() const	
	{
		return object;
	}
	//--------------------------------------------------------------------------
}
