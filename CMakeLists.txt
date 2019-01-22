set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (by default Debug)")

cmake_minimum_required(VERSION 3.7)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_HOME_DIRECTORY}/cmake ${CMAKE_HOME_DIRECTORY}/cmake/sdl)

project(winterstrike VERSION 0.4.1)

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -s")

include (FindPkgConfig)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Git)

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
    src/state.h
    src/menu.h
    src/world.h
    src/object.h
    src/character.h
    src/snowball.h
    src/label.h

    src/main.cpp
    src/sprite.cpp
    src/game.cpp
    src/menu.cpp
    src/world.cpp
    src/object.cpp
    src/character.cpp
    src/snowball.cpp
    src/label.cpp
    ${CMAKE_BINARY_DIR}/src/version.cpp
    src/res.rc
)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
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

# resources
add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/data/gfx" "${CMAKE_BINARY_DIR}/gfx"
)

add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/data/sfx" "${CMAKE_BINARY_DIR}/sfx"
)

# install
if(WIN32)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/gfx" "${CMAKE_SOURCE_DIR}/data/sfx" DESTINATION .)
    #dependencies
    file(GLOB dlls "${CMAKE_BINARY_DIR}/*.dll")
    install(FILES ${dlls} DESTINATION .)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/gfx" "${CMAKE_SOURCE_DIR}/data/sfx" DESTINATION "share/${PROJECT_NAME}")
endif()

#packaging
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

if (WIN32)
    set(CPACK_GENERATOR "NSIS" "ZIP")
    ### nsis
    set(CPACK_NSIS_COMPRESSOR bzip2)
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\WinterStrike.lnk\\\" \\\"$INSTDIR\\\\winterstrike.exe\\\""
    )
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        Delete \\\"$SMPROGRAMS\\\\$START_MENU\\\\WinterStrike.lnk\\\""
    )
else(WIN32)
    set(CPACK_GENERATOR "ZIP")
endif()

include(CPack)
