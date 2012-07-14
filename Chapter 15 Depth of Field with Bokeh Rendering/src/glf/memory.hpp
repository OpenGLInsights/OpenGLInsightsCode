#ifndef GLF_MEMORY_H
#define GLF_MEMORY_H

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/types.hpp>
#include <cstring>
//------------------------------------------------------------------------------
// Constantes
//------------------------------------------------------------------------------
// Cache line size for Core2/Nehalem/SandyBridge/NetBurst/...
// For other architecture see : http://www.agner.org/optimize/microarchitecture.pdf
#define CACHE_LINE_SIZE 64 

namespace glf
{
	//--------------------------------------------------------------------------
	template <typename T> 
	T*			AlignedAllocate(int _count);
	void*		AlignedAllocate(std::size_t _size);
	void		AlignedDesallocate(void* _ptr);
	//--------------------------------------------------------------------------
	template <typename T>
	class MemoryPool
	{
	public:
				MemoryPool(int _countMax);
				~MemoryPool();
		T*		Allocate();
		void	DesallocateAll();

	private:
				MemoryPool(const MemoryPool&);
				MemoryPool& operator=(const MemoryPool&);

	private:
		int		count;
		int		countMax;
		T* 		data;
	};
	//--------------------------------------------------------------------------
	template <class T>
	class SmartPointer
	{
	public :
				~SmartPointer();
				SmartPointer();
				SmartPointer(T* pointer);
				SmartPointer(const SmartPointer& copy);
		T& 		operator*() const;
		T* 		operator->() const;
				operator T*() const;
		const SmartPointer& operator =(const SmartPointer& copy);
		//bool	operator==(const SmartPointer<T>& _pointer) const; // Implemented with cast : operator T*()
		//bool	operator!=(const SmartPointer<T>& _pointer) const; // Implemented with cast : operator T*()

	private :
		void	Swap(SmartPointer& pointer);
		T*		object;
		int*	objectCounter; 
	};
	//--------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
// Inline include
//------------------------------------------------------------------------------
#include <glf/memory.inl>

#endif
