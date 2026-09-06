find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(vtfedit
	qt/main.cpp
	qt/MainWindow.cpp
	qt/MainWindow.h
	qt/SingleInstance.cpp
	qt/SingleInstance.h
	qt/ImageView.cpp
	qt/ImageView.h
	qt/SheetFile.cpp
	qt/SheetFile.h
	qt/SheetDialog.cpp
	qt/SheetDialog.h
	qt/RectFile.cpp
	qt/RectFile.h
	qt/HotspotDialog.cpp
	qt/HotspotDialog.h
	qt/VtfOptions.cpp
	qt/VtfOptions.h
	qt/VtfOptionsDialog.cpp
	qt/VtfOptionsDialog.h
	qt/VtfPresets.cpp
	qt/VtfPresets.h
	qt/VtfFileUtility.cpp
	qt/VtfFileUtility.h
	qt/VmtFileUtility.cpp
	qt/VmtFileUtility.h
	qt/VmtEditorOptionsDialog.cpp
	qt/VmtEditorOptionsDialog.h
	qt/VmtEditorSettings.h
	qt/VmtTextEdit.cpp
	qt/VmtTextEdit.h
	qt/VmtCreateDialog.cpp
	qt/VmtCreateDialog.h
	qt/BatchConvertDialog.cpp
	qt/BatchConvertDialog.h
	qt/BatchConvertSettings.h
	qt/AboutDialog.cpp
	qt/AboutDialog.h
	qt/FileDialogHistory.h
	qt/VTFLibQt.h
	qt/VTFEdit.rc
)

qt_add_resources(vtfedit "vtfedit_resources"
	PREFIX "/"
	BASE "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		app.ico
		icons/rgb.png
		icons/red.png
		icons/green.png
		icons/blue.png
		icons/alpha.png
		icons/alphamask.png
		icons/tile.png
)

set_target_properties(vtfedit PROPERTIES
	OUTPUT_NAME VTFEdit++
	WIN32_EXECUTABLE ON
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
	INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
	INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON
	INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON
)

target_compile_definitions(vtfedit PRIVATE
	VTFLIB_CMAKE_BUILD
	_CRT_SECURE_NO_WARNINGS
	_CRT_NONSTDC_NO_DEPRECATE
)

target_link_libraries(vtfedit PRIVATE
	vtflib
	DevIL
	Qt6::Core
	Qt6::Widgets
	Qt6::Network
)

