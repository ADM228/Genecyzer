cmake_minimum_required(VERSION 3.20)
project(Genecyzer LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_Declare(incbin
	GIT_REPOSITORY https://github.com/morswin22/cmake-incbin.git
	GIT_TAG main)
set(SFML_BUILD_NETWORK FALSE)

option(SFML_BUILD_SHARED_LIBS "Build shared SFML libraries (normally static just in case)" FALSE)
set(BUILD_SHARED_LIBS ${SFML_BUILD_SHARED_LIBS})

FetchContent_MakeAvailable(SFML incbin)

include(ExternalProject)
ExternalProject_Add(SNESFM
	PREFIX ${CMAKE_BINARY_DIR}/SNESFM
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SNESFM
	GIT_REPOSITORY https://github.com/ADM228/SNES-FM.git
	GIT_TAG master
	CONFIGURE_COMMAND ""
	BUILD_COMMAND $<$<STREQUAL:$<UPPER_CASE:${CMAKE_HOST_SYSTEM_NAME}>,LINUX>:make>
	BUILD_IN_SOURCE True
	INSTALL_COMMAND "")
	
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Genecyzer src/main.cpp)

add_library(Tile STATIC src/Tile.cpp)
target_link_libraries(Tile PUBLIC sfml-graphics)

add_library(ChrFont STATIC src/ChrFont.cpp)
target_link_libraries(ChrFont PUBLIC Tile sfml-graphics)

if (MSVC)	# Replacement of functions in cmake-incbin wrapper (it does not allow include directories, so fuck it)
	set(binIncludeFile ${CMAKE_BINARY_DIR}/binIncludes.h)
	add_custom_command(
		OUTPUT ${binIncludeFile}
		COMMAND incbin-tool -o ${binIncludeFile} -I${CMAKE_SOURCE_DIR}/SNESFM/graphics/ -I${CMAKE_SOURCE_DIR}/src/ -p bin_ -Ssnakecase main.cpp
		VERBATIM
	)
	target_sources(Genecyzer ${binIncludeFile})
	target_include_directories(Genecyzer PRIVATE ${CMAKE_BINARY_DIR})
else()
	set_target_properties(incbin-tool PROPERTIES EXCLUDE_FROM_ALL True EXCLUDE_FROM_DEFAULT_BUILD True)
	target_include_directories(Genecyzer PRIVATE "${CMAKE_SOURCE_DIR}/SNESFM/graphics")	#font location
endif()

target_link_libraries(Genecyzer PRIVATE sfml-graphics sfml-audio Tile ChrFont incbin)

if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET Genecyzer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeSFMLProject> $<TARGET_FILE_DIR:CMakeSFMLProject> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS Genecyzer)

