cmake_minimum_required(VERSION 3.21)
project(Genecyzer LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_Declare(incbin
	GIT_REPOSITORY https://github.com/morswin22/cmake-incbin.git
	GIT_TAG main)
set(SFML_BUILD_NETWORK FALSE)

FetchContent_MakeAvailable(SFML incbin)

include(ExternalProject)
ExternalProject_Add(SNESFM
	PREFIX ${CMAKE_BINARY_DIR}/SNESFM
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SNESFM
	GIT_REPOSITORY https://github.com/ADM228/SNES-FM.git
	GIT_TAG master
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE True
	INSTALL_COMMAND "")

add_executable(Genecyzer src/main.cpp)
target_link_libraries(Genecyzer PRIVATE sfml-graphics sfml-audio incbin)
target_compile_features(Genecyzer PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET Genecyzer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeSFMLProject> $<TARGET_FILE_DIR:CMakeSFMLProject> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS Genecyzer)
