cmake_minimum_required(VERSION 3.0.0)
project(netTest VERSION 0.1.0)

SET(CMAKE_CXX_STANDARD 17)

file(GLOB enet_source_files "./common/src/enet/*.c")

file(GLOB server_classes_source_files "./server/src/classes/*.cpp")
file(GLOB server_network_source_files "./server/src/network/*.cpp")
file(GLOB server_system_source_files "./server/src/system/*.cpp")

file(GLOB client_classes_source_files "./client/src/classes/*.cpp")
file(GLOB client_network_source_files "./client/src/network/*.cpp")
file(GLOB client_system_source_files "./client/src/system/*.cpp")
file(GLOB client_imgui_source_files "./client/src/3rdParty/imGui/*.cpp")
file(GLOB client_opengl_source_files "./client/src/openGL/*.cpp")
file(GLOB client_freetypegl_source_files "./client/src/3rdParty/freetype-gl/*.c")

file(GLOB common_script_source_files "./common/src/script/*.cpp")
file(GLOB common_source_files "./common/src/*.cpp")
file(GLOB common_classes_source_files "./common/src/classes/*.cpp")
file(GLOB common_tinyxml2_source_files "./common/src/tinyxml2/*.cpp")

set(SERVER_BIN_NAME_DEBUG netServer-Debug)
set(SERVER_BIN_NAME_RELEASE netServer-Release)
set(CLIENT_BIN_NAME_DEBUG netClient-Debug)
set(CLIENT_BIN_NAME_RELEASE netClient-Release)

set(SERVER_SOURCES
        server/src/main.cpp
        ${enet_source_files}
        ${server_classes_source_files}
        ${server_network_source_files}
        ${server_system_source_files}
        ${common_source_files}
        ${common_classes_source_files}
        ${common_script_source_files}
        )

set(CLIENT_SOURCES
        client/src/main.cpp
        ${enet_source_files}
        ${client_classes_source_files}
        ${client_network_source_files}
        ${client_system_source_files}
        ${client_imgui_source_files}
        ${client_opengl_source_files}
        ${client_freetypegl_source_files}
        ${common_script_source_files}
        ${common_source_files}
        ${common_classes_source_files}
        ${common_tinyxml2_source_files}
        )

include_directories(
        ./common/hdr/
        ./common/hdr/classes/
        ./common/hdr/pods/
        ./client/hdr/3rdParty/glm/
        ./client/hdr/3rdParty/freetype-gl/
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

if (WIN32)
    # Library path
    link_directories()

    link_libraries(SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)

    if (CMAKE_BUILD_TYPE MATCHES Debug)

    endif (CMAKE_BUILD_TYPE MATCHES Debug)


    if (CMAKE_BUILD_TYPE MATCHES Release)

    endif (CMAKE_BUILD_TYPE MATCHES Release)

endif (WIN32)

if (UNIX)
    # Library path
    link_directories(/usr/lib)

    link_libraries(SDL2main SDL2 SDL2_image SDL2_mixer GLEW GL GLU freetype pthread)

    include_directories(
            /usr/include
            /usr/local/include/SDL2
            /usr/include/freetype2
            ./client/hdr/3rdParty/imGui
            ./client/hdr/3rdParty/glad/
            ./common/hdr/bitsery
            ./common/hdr/tinyxml2
            ./common/hdr/script
    )

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        message("Doing DEBUG build")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g -O0 -Wimplicit-fallthrough -Wtype-limits -Wint-in-bool-context -Wcast-function-type -fno-omit-frame-pointer -Wsign-compare -fpermissive -Wall -Wextra -g -fsanitize=leak")
        add_executable(${CLIENT_BIN_NAME_DEBUG} ${CLIENT_SOURCES})
        add_executable(${SERVER_BIN_NAME_DEBUG} ${SERVER_SOURCES})
    endif (CMAKE_BUILD_TYPE MATCHES Debug)

    if (CMAKE_BUILD_TYPE MATCHES Release)
        message("Doing RELEASE build")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -DNDEBUG")
        add_executable(${CLIENT_BIN_NAME_RELEASE} ${CLIENT_SOURCES})
        add_executable(${SERVER_BIN_NAME_RELEASE} ${SERVER_SOURCES})
    endif (CMAKE_BUILD_TYPE MATCHES Release)

endif (UNIX)


