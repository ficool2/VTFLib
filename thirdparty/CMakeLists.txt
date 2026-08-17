# Third party dependencies

set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

foreach(submodule DevIL zlib libpng libjpeg-turbo Imath openexr)
	if(NOT EXISTS "${THIRDPARTY_DIR}/${submodule}/CMakeLists.txt" AND
	   NOT EXISTS "${THIRDPARTY_DIR}/${submodule}/README.md")
		message(FATAL_ERROR
			"thirdparty/${submodule} is empty. Run: git submodule update --init --recursive")
	endif()
endforeach()

# ---------------------------------------------------------------------------
# zlib
# ---------------------------------------------------------------------------

set(ZLIB_DIR "${THIRDPARTY_DIR}/zlib")

add_library(zlib_static STATIC
	"${ZLIB_DIR}/adler32.c"
	"${ZLIB_DIR}/compress.c"
	"${ZLIB_DIR}/crc32.c"
	"${ZLIB_DIR}/deflate.c"
	"${ZLIB_DIR}/gzclose.c"
	"${ZLIB_DIR}/gzlib.c"
	"${ZLIB_DIR}/gzread.c"
	"${ZLIB_DIR}/gzwrite.c"
	"${ZLIB_DIR}/infback.c"
	"${ZLIB_DIR}/inffast.c"
	"${ZLIB_DIR}/inflate.c"
	"${ZLIB_DIR}/inftrees.c"
	"${ZLIB_DIR}/trees.c"
	"${ZLIB_DIR}/uncompr.c"
	"${ZLIB_DIR}/zutil.c"
)
target_include_directories(zlib_static PUBLIC "${ZLIB_DIR}")
target_compile_definitions(zlib_static PRIVATE _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE)
set_target_properties(zlib_static PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
	FOLDER thirdparty
)

# for OpenEXR
add_library(ZLIB::ZLIB ALIAS zlib_static)

# ---------------------------------------------------------------------------
# libpng
# ---------------------------------------------------------------------------

set(PNG_DIR "${THIRDPARTY_DIR}/libpng")
set(PNG_CONF_DIR "${CMAKE_CURRENT_BINARY_DIR}/libpng-config")

configure_file(
	"${PNG_DIR}/scripts/pnglibconf.h.prebuilt"
	"${PNG_CONF_DIR}/pnglibconf.h"
	COPYONLY
)

add_library(png_static STATIC
	"${PNG_DIR}/png.c"
	"${PNG_DIR}/pngerror.c"
	"${PNG_DIR}/pngget.c"
	"${PNG_DIR}/pngmem.c"
	"${PNG_DIR}/pngpread.c"
	"${PNG_DIR}/pngread.c"
	"${PNG_DIR}/pngrio.c"
	"${PNG_DIR}/pngrtran.c"
	"${PNG_DIR}/pngrutil.c"
	"${PNG_DIR}/pngset.c"
	"${PNG_DIR}/pngtrans.c"
	"${PNG_DIR}/pngwio.c"
	"${PNG_DIR}/pngwrite.c"
	"${PNG_DIR}/pngwtran.c"
	"${PNG_DIR}/pngwutil.c"
)
target_include_directories(png_static PUBLIC "${PNG_DIR}" "${PNG_CONF_DIR}")
target_link_libraries(png_static PUBLIC zlib_static)
set_target_properties(png_static PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
	FOLDER thirdparty
)

# ---------------------------------------------------------------------------
# libjpeg-turbo
# ---------------------------------------------------------------------------

set(JPEG_DIR "${THIRDPARTY_DIR}/libjpeg-turbo")

set(ENABLE_SHARED OFF CACHE BOOL "" FORCE)
set(ENABLE_STATIC ON CACHE BOOL "" FORCE)
set(WITH_TURBOJPEG OFF CACHE BOOL "" FORCE)
set(WITH_JAVA OFF CACHE BOOL "" FORCE)
set(WITH_FUZZ OFF CACHE BOOL "" FORCE)
# Must match the runtime library DevIL is built with
set(WITH_CRT_DLL ON CACHE BOOL "" FORCE)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory("${JPEG_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/libjpeg-turbo" EXCLUDE_FROM_ALL)
unset(CMAKE_POLICY_VERSION_MINIMUM)

target_include_directories(jpeg-static INTERFACE
	"$<BUILD_INTERFACE:${JPEG_DIR}>"
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/libjpeg-turbo>"
)

set_target_properties(jpeg-static PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
	FOLDER thirdparty
)

# ---------------------------------------------------------------------------
# Imath + OpenEXR
# ---------------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

set(IMATH_INSTALL OFF CACHE BOOL "" FORCE)
set(IMATH_INSTALL_PKG_CONFIG OFF CACHE BOOL "" FORCE)
set(IMATH_INSTALL_SYM_LINK OFF CACHE BOOL "" FORCE)

set(OPENEXR_INSTALL OFF CACHE BOOL "" FORCE)
set(OPENEXR_INSTALL_TOOLS OFF CACHE BOOL "" FORCE)
set(OPENEXR_INSTALL_PKG_CONFIG OFF CACHE BOOL "" FORCE)
set(OPENEXR_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(OPENEXR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(OPENEXR_FORCE_INTERNAL_IMATH ON CACHE BOOL "" FORCE)
set(OPENEXR_FORCE_INTERNAL_ZLIB OFF CACHE BOOL "" FORCE)

add_subdirectory("${THIRDPARTY_DIR}/Imath" "${CMAKE_CURRENT_BINARY_DIR}/Imath" EXCLUDE_FROM_ALL)
add_subdirectory("${THIRDPARTY_DIR}/openexr" "${CMAKE_CURRENT_BINARY_DIR}/openexr" EXCLUDE_FROM_ALL)

foreach(openexr_target Imath ImathConfig Iex IlmThread OpenEXR OpenEXRCore OpenEXRUtil OpenEXRConfig)
	if(TARGET ${openexr_target})
		set_target_properties(${openexr_target} PROPERTIES
			POSITION_INDEPENDENT_CODE ON
			MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
			FOLDER thirdparty
		)
	endif()
endforeach()

# ---------------------------------------------------------------------------
# DevIL
# ---------------------------------------------------------------------------
set(DEVIL_DIR "${THIRDPARTY_DIR}/DevIL/DevIL")

file(GLOB DEVIL_SOURCES "${DEVIL_DIR}/src-IL/src/*.cpp")

add_library(DevIL SHARED ${DEVIL_SOURCES})

target_include_directories(DevIL
	PRIVATE
		"${THIRDPARTY_DIR}/devilconfig"  # our config.h
		"${DEVIL_DIR}/src-IL/include"
	PUBLIC
		"${DEVIL_DIR}/include"
)

target_compile_definitions(DevIL PRIVATE
	IL_DLL
	_CRT_SECURE_NO_WARNINGS
	_CRT_NONSTDC_NO_DEPRECATE
)

target_link_libraries(DevIL PRIVATE png_static jpeg-static zlib_static OpenEXR::OpenEXR)

set_source_files_properties("${DEVIL_DIR}/src-IL/src/il_exr.cpp" PROPERTIES
	COMPILE_OPTIONS "$<IF:$<CXX_COMPILER_ID:MSVC>,/FI${THIRDPARTY_DIR}/devilconfig/il_exr_compat.h,-include${THIRDPARTY_DIR}/devilconfig/il_exr_compat.h>"
)

set_target_properties(DevIL PROPERTIES
	OUTPUT_NAME "DevIL"
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
	FOLDER thirdparty
)

if(MSVC)
	target_compile_options(DevIL PRIVATE /wd4267 /wd4244 /wd4996)
endif()

# ---------------------------------------------------------------------------
# miniz
# ---------------------------------------------------------------------------

set(MINIZ_DIR "${THIRDPARTY_DIR}/miniz")
set(MINIZ_CONF_DIR "${CMAKE_CURRENT_BINARY_DIR}/miniz-config")

file(WRITE "${MINIZ_CONF_DIR}/miniz_export.h"
	"#ifndef MINIZ_EXPORT\n#define MINIZ_EXPORT\n#endif\n")

add_library(miniz STATIC
	"${MINIZ_DIR}/miniz.c"
	"${MINIZ_DIR}/miniz_tdef.c"
	"${MINIZ_DIR}/miniz_tinfl.c"
	"${MINIZ_DIR}/miniz_zip.c"
)
target_include_directories(miniz PUBLIC "${MINIZ_DIR}" "${MINIZ_CONF_DIR}")
target_compile_definitions(miniz PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
set_target_properties(miniz PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	FOLDER thirdparty
)

# ---------------------------------------------------------------------------
# zstd
# ---------------------------------------------------------------------------

set(ZSTD_DIR "${THIRDPARTY_DIR}/zstd/lib")

file(GLOB ZSTD_SOURCES
	"${ZSTD_DIR}/common/*.c"
	"${ZSTD_DIR}/compress/*.c"
	"${ZSTD_DIR}/decompress/*.c"
)

add_library(zstd STATIC ${ZSTD_SOURCES})
target_include_directories(zstd
	PRIVATE "${ZSTD_DIR}/common"
	PUBLIC "${ZSTD_DIR}"
)
target_compile_definitions(zstd
	PRIVATE
		ZSTD_LEGACY_SUPPORT=0
		$<$<BOOL:${MSVC}>:ZSTD_DISABLE_ASM>
		_CRT_SECURE_NO_WARNINGS
)
set_target_properties(zstd PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	FOLDER thirdparty
)

# ---------------------------------------------------------------------------
# Compressonator
# ---------------------------------------------------------------------------

set(CMP_DIR "${THIRDPARTY_DIR}/compressonator")

# The SIMD kernels are compiled separately, each with its own ISA flags
foreach(simd SSE AVX AVX512)
	string(TOLOWER "${simd}" simd_lower)
	add_library(CMP_Core_${simd} STATIC "${CMP_DIR}/cmp_core/source/core_simd_${simd_lower}.cpp")
	target_include_directories(CMP_Core_${simd} PRIVATE
		"${CMP_DIR}/cmp_core/source"
		"${CMP_DIR}/cmp_core/shaders"
	)
	set_target_properties(CMP_Core_${simd} PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
		FOLDER thirdparty
	)
endforeach()

if(MSVC)
	target_compile_options(CMP_Core_AVX PRIVATE /arch:AVX2)
	target_compile_options(CMP_Core_AVX512 PRIVATE /arch:AVX512)
endif()

add_library(CMP_Core STATIC
	"${CMP_DIR}/cmp_core/shaders/bc1_encode_kernel.cpp"
	"${CMP_DIR}/cmp_core/shaders/bc2_encode_kernel.cpp"
	"${CMP_DIR}/cmp_core/shaders/bc3_encode_kernel.cpp"
	"${CMP_DIR}/cmp_core/shaders/bc4_encode_kernel.cpp"
	"${CMP_DIR}/cmp_core/shaders/bc5_encode_kernel.cpp"
	"${CMP_DIR}/cmp_core/shaders/bc6_encode_kernel.cpp"
	"${CMP_DIR}/cmp_core/shaders/bc7_encode_kernel.cpp"
	"${CMP_DIR}/cmp_core/source/cmp_core.cpp"
	"${CMP_DIR}/applications/_libs/cmp_math/cpu_extensions.cpp"
	"${CMP_DIR}/applications/_libs/cmp_math/cmp_math_common.cpp"
)
target_include_directories(CMP_Core PUBLIC
	"${CMP_DIR}/cmp_core/shaders"
	"${CMP_DIR}/cmp_core/source"
	"${CMP_DIR}/applications/_libs/cmp_math"
)
target_link_libraries(CMP_Core PRIVATE CMP_Core_SSE CMP_Core_AVX CMP_Core_AVX512)
set_target_properties(CMP_Core PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	FOLDER thirdparty
)

file(GLOB_RECURSE CMP_COMPRESSONATOR_SOURCES
	"${CMP_DIR}/cmp_compressonatorlib/apc/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/atc/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/ati/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/ati/*.c"
	"${CMP_DIR}/cmp_compressonatorlib/basis/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/bc6h/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/bc7/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/block/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/buffer/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/dxt/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/dxtc/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/dxtc/*.c"
	"${CMP_DIR}/cmp_compressonatorlib/etc/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/etc/etcpack/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/etc/etcpack/*.cxx"
	"${CMP_DIR}/cmp_compressonatorlib/gt/*.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/common/*.cpp"
	"${CMP_DIR}/cmp_framework/common/*.cpp"
	"${CMP_DIR}/cmp_framework/common/half/*.cpp"
)

add_library(CMP_Compressonator STATIC
	${CMP_COMPRESSONATOR_SOURCES}
	"${CMP_DIR}/cmp_compressonatorlib/compress.cpp"
	"${CMP_DIR}/cmp_compressonatorlib/compressonator.cpp"
	"${CMP_DIR}/applications/_plugins/common/atiformats.cpp"
	"${CMP_DIR}/applications/_plugins/common/format_conversion.cpp"
	"${CMP_DIR}/applications/_plugins/common/codec_common.cpp"
	"${CMP_DIR}/applications/_plugins/common/texture_utils.cpp"
)

target_include_directories(CMP_Compressonator PUBLIC
	"${CMP_DIR}/cmp_compressonatorlib"
	"${CMP_DIR}/cmp_compressonatorlib/apc"
	"${CMP_DIR}/cmp_compressonatorlib/atc"
	"${CMP_DIR}/cmp_compressonatorlib/ati"
	"${CMP_DIR}/cmp_compressonatorlib/basis"
	"${CMP_DIR}/cmp_compressonatorlib/bc6h"
	"${CMP_DIR}/cmp_compressonatorlib/bc7"
	"${CMP_DIR}/cmp_compressonatorlib/block"
	"${CMP_DIR}/cmp_compressonatorlib/buffer"
	"${CMP_DIR}/cmp_compressonatorlib/dxt"
	"${CMP_DIR}/cmp_compressonatorlib/dxtc"
	"${CMP_DIR}/cmp_compressonatorlib/etc"
	"${CMP_DIR}/cmp_compressonatorlib/etc/etcpack"
	"${CMP_DIR}/cmp_compressonatorlib/gt"
	"${CMP_DIR}/cmp_compressonatorlib/common"
	"${CMP_DIR}/cmp_framework/common"
	"${CMP_DIR}/cmp_framework/common/half"
	"${CMP_DIR}/applications/_plugins/common"
	"${CMP_DIR}/applications/_libs/cmp_math"
)

target_link_libraries(CMP_Compressonator PUBLIC CMP_Core)

set_target_properties(CMP_Compressonator PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	FOLDER thirdparty
)
