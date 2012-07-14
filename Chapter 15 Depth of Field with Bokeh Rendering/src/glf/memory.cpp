//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/memory.hpp>
#include <malloc.h>
namespace glf
{
	//--------------------------------------------------------------------------
	// From PBRT v2.0 (http://www.pbrt.org)
	void* AlignedAllocate(std::size_t _size)
	{
		#if defined(WIN32)
		return _aligned_malloc(_size, CACHE_LINE_SIZE);
		#else
		return memalign(CACHE_LINE_SIZE, _size);
		#endif
	}
	//--------------------------------------------------------------------------
	// From PBRT v2.0 (http://www.pbrt.org)
	void AlignedDesallocate(void* _ptr)
	{
		if (!_ptr) return;
		#if defined(WIN32)
		_aligned_free(_ptr);
		#else
		free(_ptr);
		#endif
	}
	//--------------------------------------------------------------------------
}

