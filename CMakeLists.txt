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

set_target_properties(incbin-tool PROPERTIES EXCLUDE_FROM_ALL True EXCLUDE_FROM_DEFAULT_BUILD True)

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

add_library(cpp_std INTERFACE)
target_compile_features(cpp_std INTERFACE cxx_std_17)
	
add_executable(Genecyzer src/main.cpp)
add_library(Tile STATIC src/Tile.cpp)
target_link_libraries(Tile PUBLIC sfml-graphics INTERFACE cpp_std)
add_library(ChrFont STATIC src/ChrFont.cpp)
target_link_libraries(ChrFont PUBLIC Tile sfml-graphics INTERFACE cpp_std)
target_link_directories(Genecyzer PRIVATE "${CMAKE_SOURCE_DIR}/SNESFM/graphics")	#for the font
target_link_libraries(Genecyzer PRIVATE sfml-graphics sfml-audio incbin Tile ChrFont INTERFACE cpp_std)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET Genecyzer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeSFMLProject> $<TARGET_FILE_DIR:CMakeSFMLProject> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS Genecyzer)

