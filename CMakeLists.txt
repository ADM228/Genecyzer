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

message("* Getting SFML...")
FetchContent_MakeAvailable(SFML)

message("* Getting incbin...")
FetchContent_MakeAvailable(incbin)

message("* Getting clip...")
FetchContent_MakeAvailable(clip)

message("* Libraries acquired")


option(SFML_BUILD_SHARED_LIBS "Build shared SFML libraries (normally static just in case)" FALSE)
set(BUILD_SHARED_LIBS ${SFML_BUILD_SHARED_LIBS})
set(CLIP_EXAMPLES FALSE)
set(CLIP_TESTS FALSE)
if (UNIX AND NOT APPLE)
	set(CLIP_X11_WITH_PNG FALSE)
endif()

set(THIRDPARTY "${CMAKE_SOURCE_DIR}/thirdparty/")

include(ExternalProject)
ExternalProject_Add(SNESFM
	GIT_REPOSITORY https://github.com/ADM228/SNES-FM.git
	GIT_TAG master
	CONFIGURE_COMMAND ""
	BUILD_COMMAND $<$<STREQUAL:$<UPPER_CASE:${CMAKE_HOST_SYSTEM_NAME}>,LINUX>:make>
	BUILD_IN_SOURCE True
	INSTALL_COMMAND "")
ExternalProject_Add(tinyfiledialogs
	GIT_REPOSITORY https://git.code.sf.net/p/tinyfiledialogs/code
	GIT_TAG master
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND "")

ExternalProject_Get_Property(SNESFM SOURCE_DIR)
set (SNESFM_SOURCE_DIR "${SOURCE_DIR}")
ExternalProject_Get_Property(tinyfiledialogs SOURCE_DIR)
set (TINYFD_SOURCE_DIR "${SOURCE_DIR}")

add_executable(Genecyzer src/main.cpp)
target_compile_features(Genecyzer PRIVATE cxx_std_17)

add_custom_target(GenecyzerFileTest DEPENDS Genecyzer)

if (TARGET GenecyzerFileTest)
	target_compile_definitions(Genecyzer PRIVATE FILETEST)
	configure_file("${CMAKE_SOURCE_DIR}/tests/testFile.gczr" "${CMAKE_BINARY_DIR}/testFile.gczr" COPYONLY)
endif()

set(FONTFILE "tilesetUnicode.chr")
set(FONTDIR "${SNESFM_SOURCE_DIR}/graphics/")

set(BININCLUDEFILE ${CMAKE_BINARY_DIR}/binIncludes.cpp)
set(MSVCINCLUDEFILE ${CMAKE_BINARY_DIR}/MSVCIncludes.h)

set(INCBIN_PREFIX bin_)
set(INCBIN_STYLE INCBIN_STYLE_SNAKE)
set(INCBIN_TOOL_STYLE snakecase)

configure_file("${CMAKE_SOURCE_DIR}/src/binIncludes.cpp.in" "${BININCLUDEFILE}")
target_include_directories(Genecyzer PRIVATE ${CMAKE_BINARY_DIR} ${THIRDPARTY} ${TINYFD_SOURCE_DIR})

add_library(Font STATIC "${BININCLUDEFILE}")
add_custom_command(OUTPUT "${FONTDIR}" "${FONTDIR}${FONTFILE}" DEPENDS SNESFM COMMAND "")
add_custom_command(OUTPUT "${TINYFD_SOURCE_DIR}" "${TINYFD_SOURCE_DIR}/tinyfiledialogs.c" DEPENDS tinyfiledialogs COMMAND "")
target_sources(Font PUBLIC "${FONTDIR}${FONTFILE}")
target_link_libraries(Font PUBLIC incbin)

add_library(riff STATIC "${THIRDPARTY}/libriff/riff.c")
add_library(tinyfd STATIC "${TINYFD_SOURCE_DIR}/tinyfiledialogs.c")

target_compile_definitions(riff PUBLIC RIFF_WRITE)
target_compile_features(riff PRIVATE c_std_99)

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


target_link_libraries(Genecyzer PRIVATE sfml-graphics sfml-audio Font clip riff tinyfd)

if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET Genecyzer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeSFMLProject> $<TARGET_FILE_DIR:CMakeSFMLProject> COMMAND_EXPAND_LISTS)
endif()

# install(TARGETS Genecyzer CONFIGURATIONS Debug
	# RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
