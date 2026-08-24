add_executable(vtf_fuzz vtf_fuzz.cpp)

target_include_directories(vtf_fuzz PRIVATE
	"${CMAKE_SOURCE_DIR}/VTFLib"
	"${VTFLIB_THIRDPARTY}/include"
)

target_link_libraries(vtf_fuzz PRIVATE vtflib)
