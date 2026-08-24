add_library(vtflib SHARED
	Error.cpp
	FileReader.cpp
	FileWriter.cpp
	Float16.cpp
	MemoryReader.cpp
	MemoryWriter.cpp
	Proc.cpp
	ProcReader.cpp
	ProcWriter.cpp
	VMTFile.cpp
	VMTGroupNode.cpp
	VMTIntegerNode.cpp
	VMTNode.cpp
	VMTSingleNode.cpp
	VMTStringNode.cpp
	VMTValueNode.cpp
	VMTWrapper.cpp
	VTFDistanceAlpha.cpp
	VTFFile.cpp
	VTFLib.cpp
	VTFMathlib.cpp
	VTFWrapper.cpp
)

if(WIN32)
	target_sources(vtflib PRIVATE Resource.rc)
endif()

set_target_properties(vtflib PROPERTIES
	# Deliberately not "VTFLib"!
	# third-party VTF shell extensions get loaded into our process by the shell
	# and would attempt to use our VTFLib, which has a fatal ABI mismatch
	OUTPUT_NAME VTFLib++
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_compile_definitions(vtflib
	PRIVATE
		VTFLIB_EXPORTS
		$<$<BOOL:${WIN32}>:_WINDOWS>
		$<$<BOOL:${WIN32}>:_USRDLL>
		_CRT_SECURE_NO_WARNINGS
		_CRT_NONSTDC_NO_DEPRECATE
		$<$<NOT:$<CONFIG:Debug>>:_USE_RTM_VERSION>
)

set_target_properties(vtflib PROPERTIES
	C_VISIBILITY_PRESET hidden
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
)

target_include_directories(vtflib
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${VTFLIB_THIRDPARTY}/include"
	INTERFACE
		"${CMAKE_SOURCE_DIR}/lib"
)

target_link_libraries(vtflib PRIVATE
	miniz
	zstd
	CMP_Compressonator
)
