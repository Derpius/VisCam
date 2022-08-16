cmake_minimum_required (VERSION 3.20)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project("VisCam" CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP QUIET)

add_subdirectory("libs/glm")

add_library(
	${PROJECT_NAME} SHARED
	"source/VisCam.cpp"
	"source/Bokeh.cpp"
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	"source"
	"libs/glm"
	"libs/gmod-module-base/include"
	"libs/VisTrace/include"
)

if (OpenMP_CXX_FOUND)
	target_link_libraries(
		${PROJECT_NAME} PRIVATE
		OpenMP::OpenMP_CXX
		libomp.lib
	)
endif()

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	glm
)
