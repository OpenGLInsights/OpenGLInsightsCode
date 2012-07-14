#ifndef CONFIG_H
#define CONFIG_H

#ifndef USE_CMAKE_VALUES

	#define INDEX_32BITS
	//#define INDEX_16BITS

	//#define INDEXED_NONINTERLEAVED
	//#define NONINDEXED_NONINTERLEAVED
	#define INDEXED_INTERLEAVED
	//#define NONINDEXED_INTERLEAVED

	//#define OPTIMIZE // requires INDEXED_***

#endif

#ifdef INDEX_32BITS
// Uncompatible with NvTriStrip ( #define OPTIMIZE ), but compatible with all provided meshes
typedef unsigned int indextype;
#define INDEX_TYPE_GL GL_UNSIGNED_INT
#endif

#ifdef INDEX_16BITS
// > 64000 vertices meshes will be broken, but may be optimized
typedef unsigned short indextype;
#define INDEX_TYPE_GL GL_UNSIGNED_SHORT
#endif

#endif