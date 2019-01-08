set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (by default Debug)")

cmake_minimum_required(VERSION 3.7)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_HOME_DIRECTORY}/cmake ${CMAKE_HOME_DIRECTORY}/cmake/sdl)

project(winterstrike)
set(winterstrike_VERSION 0.2.0)

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -s")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-longc -fno-rtti -O3")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include (FindPkgConfig)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Git)

# resources
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/sfx")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/gfx")

configure_file("${CMAKE_SOURCE_DIR}/data/LinBiolinum_Rah.ttf" "${CMAKE_BINARY_DIR}/gfx/LinBiolinum_Rah.ttf" COPYONLY)

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/gfx/character.png
    DEPENDS ${CMAKE_SOURCE_DIR}/data/character.blend
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts/render.py 
    COMMAND blender -b "${CMAKE_SOURCE_DIR}/data/character.blend" -P "${CMAKE_SOURCE_DIR}/scripts/render.py" -- "${CMAKE_BINARY_DIR}/gfx/character.png" 8
)
add_custom_target(character ALL DEPENDS gfx/character.png)

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/gfx/snowball.png
    DEPENDS ${CMAKE_SOURCE_DIR}/data/snowball.blend
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts/render.py 
    COMMAND blender -b "${CMAKE_SOURCE_DIR}/data/snowball.blend" --log-level 0 -P "${CMAKE_SOURCE_DIR}/scripts/render.py" -- "${CMAKE_BINARY_DIR}/gfx/snowball.png" 1
)
add_custom_target(snowball ALL DEPENDS gfx/snowball.png)

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/gfx/tiles.png
    DEPENDS ${CMAKE_SOURCE_DIR}/data/tiles.blend
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts/render.py 
    COMMAND blender -b "${CMAKE_SOURCE_DIR}/data/tiles.blend" --log-level 0 -P "${CMAKE_SOURCE_DIR}/scripts/render.py" -- "${CMAKE_BINARY_DIR}/gfx/tiles.png" 1
)
add_custom_target(tiles ALL DEPENDS gfx/tiles.png)

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/sfx/music.ogg
    DEPENDS ${CMAKE_SOURCE_DIR}/data/music.mid
    COMMAND fluidsynth -nli -r 48000 -T oga -F "${CMAKE_BINARY_DIR}/sfx/music.ogg" "/usr/share/soundfonts/FluidR3_GM.sf2" "${CMAKE_SOURCE_DIR}/data/music.mid"
)
add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/sfx/hit.ogg
    DEPENDS ${CMAKE_SOURCE_DIR}/data/hit.mid
    COMMAND fluidsynth -nli -r 48000 -T oga -F "${CMAKE_BINARY_DIR}/sfx/hit.ogg" "/usr/share/soundfonts/FluidR3_GM.sf2" "${CMAKE_SOURCE_DIR}/data/hit.mid"
)
add_custom_target(sfx ALL DEPENDS sfx/music.ogg sfx/hit.ogg) 

# application
include_directories(
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME}
	src/game.h
	src/sprite.h
	src/world.h
	src/camera.h
	src/object.h
	src/character.h
	src/label.h

	src/main.cpp
	src/game.cpp
	src/sprite.cpp
	src/world.cpp
    src/object.cpp
	src/camera.cpp
	src/character.cpp
    src/snowball.cpp
    src/label.cpp
	${CMAKE_BINARY_DIR}/src/version.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

# version and build time tracker
execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE PROJECT_COMMIT
	OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
configure_file(
	"${CMAKE_SOURCE_DIR}/src/version.cpp.in"
	"${CMAKE_BINARY_DIR}/src/version.cpp"
	@ONLY
)
add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/src/version.cpp
    # COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target linkdate
)

# install
if(WIN32)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
	install(DIRECTORY "${CMAKE_BINARY_DIR}/gfx" "${CMAKE_BINARY_DIR}/sfx" DESTINATION .)
    #dependencies
    file(GLOB dlls "${CMAKE_BINARY_DIR}/*.dll")
    install(FILES ${dlls} DESTINATION .)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
	install(DIRECTORY "${CMAKE_BINARY_DIR}/gfx" "${CMAKE_BINARY_DIR}/sfx" DESTINATION "share/${PROJECT_NAME}")
endif()

#packaging
set(CPACK_GENERATOR "NSIS" "ZIP")

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Kumok Boris")
set(CPACK_PACKAGE_CONTACT qborki@gmail.com)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "winterstrike is an isometric snowball fighting game. Just a small hobby project to learn SDL and C++.")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_HOME_DIRECTORY}/README.md)

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}")
string(TOLOWER ${CPACK_PACKAGE_FILE_NAME} CPACK_PACKAGE_FILE_NAME)
set(CPACK_MONOLITHIC_INSTALL ON)

### nsis
set(CPACK_NSIS_COMPRESSOR bzip2)
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\WinterStrike.lnk\\\" \\\"$INSTDIR\\\\winterstrike.exe\\\""
)
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
	Delete \\\"$SMPROGRAMS\\\\$START_MENU\\\\WinterStrike.lnk\\\""
)

### source
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-v${CPACK_PACKAGE_VERSION}-${PROJECT_COMMIT}-source")
set(CPACK_SOURCE_IGNORE_FILES
    "~$"
    "\\\\.asm$"
    "\\\\.bz2$"
    "/CMakeCache\\\\.txt$"
    "/CMakeFiles/"
    "/cmake_install\\\\.cmake$"
    "/CPackConfig.cmake$"
    "/_CPack_Packages/"
    "/CPackSourceConfig.cmake$"
    "\\\\.dll$"
    "\\\\.exe$"
    "\\\\.exp$"
    "\\\\.git/"
    "\\\\.gz$"
    "\\\\.lib$"
    "/Makefile$"
    "\\\\.pc$"
    "\\\\.pdb$"
    "\\\\.pyc$"
    "/build/"
    "\\\\.so$"
    "\\\\.swp$"
)

set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-v${FreeOrion_VERSION}-${FreeOrion_WC_REVISION}-source"
)
include(CPack)
