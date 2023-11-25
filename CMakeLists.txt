cmake_minimum_required(VERSION 3.20)
project(Genecyzer LANGUAGES CXX C)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
set(SFML_BUILD_NETWORK FALSE)

FetchContent_Declare(incbin
	GIT_REPOSITORY https://github.com/morswin22/cmake-incbin.git
	GIT_TAG main)

cmake_policy(SET CMP0077 NEW)
FetchContent_Declare(clip
	GIT_REPOSITORY https://github.com/dacap/clip.git
	GIT_TAG v1.6)
set(CLIP_EXAMPLES FALSE)
set(CLIP_TESTS FALSE)
if (UNIX AND NOT APPLE)
	set(CLIP_X11_WITH_PNG FALSE)
endif()


option(SFML_BUILD_SHARED_LIBS "Build shared SFML libraries (normally static just in case)" FALSE)
set(BUILD_SHARED_LIBS ${SFML_BUILD_SHARED_LIBS})

FetchContent_MakeAvailable(SFML incbin clip)

include(ExternalProject)
ExternalProject_Add(SNESFM
	PREFIX ${CMAKE_BINARY_DIR}/SNESFM
	SOURCE_DIR ${CMAKE_BINARY_DIR}/SNESFM/source
	GIT_REPOSITORY https://github.com/ADM228/SNES-FM.git
	GIT_TAG master
	CONFIGURE_COMMAND ""
	BUILD_COMMAND $<$<STREQUAL:$<UPPER_CASE:${CMAKE_HOST_SYSTEM_NAME}>,LINUX>:make>
	BUILD_IN_SOURCE True
	INSTALL_COMMAND "")

add_executable(Genecyzer src/main.cpp)
target_compile_features(Genecyzer PRIVATE cxx_std_17)

set(FONTFILE "tilesetUnicode.chr")
set(FONTDIR "${CMAKE_BINARY_DIR}/SNESFM/source/graphics/")

set(THIRDPARTY "${CMAKE_SOURCE_DIR}/thirdparty/")

set(BININCLUDEFILE ${CMAKE_BINARY_DIR}/binIncludes.cpp)
set(MSVCINCLUDEFILE ${CMAKE_BINARY_DIR}/MSVCIncludes.h)

set(INCBIN_PREFIX bin_)
set(INCBIN_STYLE INCBIN_STYLE_SNAKE)
set(INCBIN_TOOL_STYLE snakecase)

configure_file("${CMAKE_SOURCE_DIR}/src/binIncludes.cpp.in" "${BININCLUDEFILE}")
target_include_directories(Genecyzer PRIVATE ${CMAKE_BINARY_DIR} ${THIRDPARTY})

add_library(Font STATIC "${BININCLUDEFILE}")
add_custom_command(OUTPUT "${FONTDIR}" "${FONTDIR}${FONTFILE}" DEPENDS SNESFM COMMAND "")
target_sources(Font PUBLIC "${FONTDIR}${FONTFILE}")
target_link_libraries(Font PUBLIC incbin)

add_library(libriff STATIC "${THIRDPARTY}/libriff/riff.c")

if (MSVC)	# Replacement of functions in cmake-incbin wrapper (it does not allow include directories, so fuck it)
	get_filename_component(BININCLUDEFILE_NAME ${BININCLUDEFILE} NAME)
	get_filename_component(BININCLUDEFILE_DIR ${BININCLUDEFILE} DIRECTORY)
	add_custom_command(
		OUTPUT ${MSVCINCLUDEFILE}
		COMMAND incbin-tool -o ${MSVCINCLUDEFILE} -I${FONTDIR} -I${BININCLUDEFILE_DIR} -p ${INCBIN_PREFIX} -S${INCBIN_TOOL_STYLE} "${BININCLUDEFILE_NAME}"
		DEPENDS "${FONTDIR}${FONTFILE}" "${BININCLUDEFILE}"
		VERBATIM
	)
	target_sources(Font PUBLIC ${MSVCINCLUDEFILE})
else()
	target_include_directories(Font PUBLIC "${FONTDIR}")
	set_target_properties(incbin-tool PROPERTIES EXCLUDE_FROM_ALL True EXCLUDE_FROM_DEFAULT_BUILD True)
endif()


target_link_libraries(Genecyzer PRIVATE sfml-graphics sfml-audio Font clip libriff)

if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET Genecyzer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeSFMLProject> $<TARGET_FILE_DIR:CMakeSFMLProject> COMMAND_EXPAND_LISTS)
endif()

# install(TARGETS Genecyzer CONFIGURATIONS Debug
	# RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
