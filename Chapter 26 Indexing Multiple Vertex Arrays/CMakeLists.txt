# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (OpenGLInsights_Indexing)

find_package(OpenGL)

# Compile external dependencies 
add_subdirectory (external)

include_directories(
	external/glfw-2.7/include/
	external/glm-0.9.1/
	external/glew-1.5.8/include/
	external/NvTriStrip/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	GLFW_27
	GLEW_158
	NvTriStrip
)

add_definitions(
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

set( ArticleFiles
	Article/Article.cpp
	Article/vboindexer.cpp
	Article/vboindexer.hpp
	Article/packedvertex.hpp
	Article/config.hpp
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
	common/texture.cpp
	common/texture.hpp
	common/objloader.cpp
	common/objloader.hpp
)

MACRO(ADDVERSION VERSIONNAME VERSIONPARAMS)
	add_executable(${VERSIONNAME} ${ArticleFiles})
	set_target_properties(${VERSIONNAME} PROPERTIES COMPILE_FLAGS ${VERSIONPARAMS} )
	target_link_libraries(${VERSIONNAME} ${ALL_LIBS})
	add_custom_command(
	   TARGET ${VERSIONNAME} POST_BUILD
	   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${VERSIONNAME}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/article/"
	)
ENDMACRO(ADDVERSION)


ADDVERSION(Article_32Bits_Indexed_Noninterleaved_NonOptimized    "-DUSE_CMAKE_VALUES -DINDEXED_NONINTERLEAVED    -DINDEX_32BITS")
ADDVERSION(Article_32Bits_Indexed_Interleaved_NonOptimized       "-DUSE_CMAKE_VALUES -DINDEXED_INTERLEAVED       -DINDEX_32BITS")
ADDVERSION(Article_32Bits_Nonindexed_Noninterleaved_NonOptimized "-DUSE_CMAKE_VALUES -DNONINDEXED_NONINTERLEAVED -DINDEX_32BITS")
ADDVERSION(Article_32Bits_Nonindexed_Interleaved_NonOptimized    "-DUSE_CMAKE_VALUES -DNONINDEXED_INTERLEAVED    -DINDEX_32BITS")

ADDVERSION(Article_16Bits_Indexed_Noninterleaved_NonOptimized    "-DUSE_CMAKE_VALUES -DINDEXED_NONINTERLEAVED    -DINDEX_16BITS")
ADDVERSION(Article_16Bits_Indexed_Interleaved_NonOptimized       "-DUSE_CMAKE_VALUES -DINDEXED_INTERLEAVED       -DINDEX_16BITS")
ADDVERSION(Article_16Bits_Nonindexed_Noninterleaved_NonOptimized "-DUSE_CMAKE_VALUES -DNONINDEXED_NONINTERLEAVED -DINDEX_16BITS")
ADDVERSION(Article_16Bits_Nonindexed_Interleaved_NonOptimized    "-DUSE_CMAKE_VALUES -DNONINDEXED_INTERLEAVED    -DINDEX_16BITS")

# NvTriStrip requires indexing (obviously) and 16 bit indices
# Other algorithms don't necessarily have such limitations
ADDVERSION(Article_16Bits_Indexed_Noninterleaved_Optimized       "-DUSE_CMAKE_VALUES -DINDEXED_NONINTERLEAVED    -DINDEX_16BITS -DOPTIMIZE")
ADDVERSION(Article_16Bits_Indexed_Interleaved_Optimized          "-DUSE_CMAKE_VALUES -DINDEXED_INTERLEAVED       -DINDEX_16BITS -DOPTIMIZE")

# Custom version
ADDVERSION(Article_Custom "-DUSE_YOUR_OWN_DEFINES")

