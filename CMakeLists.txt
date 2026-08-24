cmake_minimum_required(VERSION 3.21)

project(VTFLib
	VERSION 2.0.0
	LANGUAGES C CXX
)

# zstd ships hand-written x86-64 assembly for its decompressor
if(NOT MSVC)
	enable_language(ASM)
endif()

include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VTFLIB_BUILD_VTFCMD "Build VTFCmd" ON)
option(VTFLIB_BUILD_FUZZER "Build VTFFuzzer" OFF)
option(VTFLIB_BUILD_VTFEDIT "Build VTFEdit" ON)

foreach(config "" _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY${config} "${CMAKE_BINARY_DIR}/bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY${config} "${CMAKE_BINARY_DIR}/bin")
endforeach()

set(VTFLIB_THIRDPARTY "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

add_subdirectory(thirdparty)

add_subdirectory(VTFLib)

if(VTFLIB_BUILD_VTFCMD)
	add_subdirectory(VTFCmd)
endif()

if(VTFLIB_BUILD_VTFEDIT)
	add_subdirectory(VTFEdit)
endif()

if(VTFLIB_BUILD_FUZZER)
	add_subdirectory(fuzz)
endif()
