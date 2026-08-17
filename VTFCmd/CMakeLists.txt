add_executable(vtfcmd
	vtfcmd.c
	enumerations.c
	VTFCmd.rc
)

set_target_properties(vtfcmd PROPERTIES
	OUTPUT_NAME VTFCmd
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_compile_definitions(vtfcmd PRIVATE
	VTFLIB_CMAKE_BUILD
	_CONSOLE
	_CRT_SECURE_NO_WARNINGS
	_CRT_NONSTDC_NO_DEPRECATE
	$<$<NOT:$<CONFIG:Debug>>:_USE_RTM_VERSION>
)

target_include_directories(vtfcmd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(vtfcmd PRIVATE vtflib DevIL)

