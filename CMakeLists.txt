cmake_minimum_required(VERSION 3.28)
project(Genecyzer LANGUAGES CXX C)

# set(SFML_MANUAL_OVERRIDE TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT SFML_MANUAL_OVERRIDE)
	set(SFML_STATIC_LIBRARIES FALSE)
	find_package(SFML 3 COMPONENTS Graphics Audio Window System)
endif()

include(FetchContent)
if (NOT SFML_FOUND)
	FetchContent_Declare(SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG 3.0.1
		GIT_SHALLOW ON
		EXCLUDE_FROM_ALL
    	SYSTEM)
	set(SFML_BUILD_NETWORK FALSE)
endif()

FetchContent_Declare(incbin
	GIT_REPOSITORY https://github.com/morswin22/cmake-incbin.git
	GIT_TAG main)

# FetchContent_Declare(clip
# 	GIT_REPOSITORY https://github.com/dacap/clip.git
# 	GIT_TAG v1.10)

set(RIFF_STATIC_LIBRARIES TRUE)
set(RIFF_WRITE TRUE)
set(RIFF_CXX_WRAPPER TRUE)

FetchContent_Declare(riff
	GIT_REPOSITORY https://github.com/ADM228/libriff.git
	GIT_TAG genecyzer-version)

if (NOT SFML_FOUND)
	message("* Getting SFML...")
	FetchContent_MakeAvailable(SFML)
endif()

message("* Getting incbin...")
FetchContent_MakeAvailable(incbin)

#message("* Getting clip...")
#FetchContent_MakeAvailable(clip)

message("* Getting libriff...")
FetchContent_MakeAvailable(riff)

message("* Libraries acquired")

set(BUILD_SHARED_LIBS FALSE)
# set(CLIP_EXAMPLES FALSE)
# set(CLIP_TESTS FALSE)
# if (UNIX AND NOT APPLE)
# 	set(CLIP_X11_WITH_PNG FALSE)
# endif()

include(ExternalProject)
ExternalProject_Add(SNESFM
	GIT_REPOSITORY https://github.com/ADM228/SNES-FM.git
	GIT_TAG master
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
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

target_compile_definitions(Genecyzer PRIVATE BITCONVERTER_ARRAY_CONVS BITCONVERTER_VECTOR_CONVS)

set(FONTFILE "tilesetUnicode.chr")
set(FONTDIR "${SNESFM_SOURCE_DIR}/graphics/")

set(BININCLUDEFILE ${CMAKE_BINARY_DIR}/binIncludes.cpp)
set(MSVCINCLUDEFILE ${CMAKE_BINARY_DIR}/MSVCIncludes.h)

set(INCBIN_PREFIX bin_)
set(INCBIN_STYLE INCBIN_STYLE_SNAKE)
set(INCBIN_TOOL_STYLE snakecase)

configure_file("${CMAKE_SOURCE_DIR}/src/binIncludes.cpp.in" "${BININCLUDEFILE}")
target_include_directories(Genecyzer PRIVATE ${CMAKE_BINARY_DIR} ${TINYFD_SOURCE_DIR})

add_library(Font STATIC "${BININCLUDEFILE}")
add_custom_command(OUTPUT "${FONTDIR}" "${FONTDIR}${FONTFILE}" DEPENDS SNESFM COMMAND "")
add_custom_command(OUTPUT "${TINYFD_SOURCE_DIR}" "${TINYFD_SOURCE_DIR}/tinyfiledialogs.c" DEPENDS tinyfiledialogs COMMAND "")
target_sources(Font PUBLIC "${FONTDIR}${FONTFILE}")
target_link_libraries(Font PUBLIC incbin)

add_library(tinyfd STATIC "${TINYFD_SOURCE_DIR}/tinyfiledialogs.c")

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

target_include_directories(Genecyzer PRIVATE src)
target_link_libraries(Genecyzer PRIVATE SFML::Graphics SFML::Audio Font riff tinyfd)

# install(TARGETS Genecyzer CONFIGURATIONS Debug
	# RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
