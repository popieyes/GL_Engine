cmake_minimum_required(VERSION 3.10)
project(GL_Engine)

# Prefer GLVND OpenGL over legacy libGL when both are present
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# On Linux, apt packages expose pkg-config files but not always CMake config files.
# Use pkg_check_modules (IMPORTED_TARGET) for glfw3 and yaml-cpp on Linux.
if(WIN32)
    find_package(glfw3 3.3 REQUIRED)
    find_package(glm REQUIRED)
    find_package(assimp REQUIRED)
    find_package(yaml-cpp CONFIG REQUIRED)

    set(PLATFORM_LIBS glfw glm::glm assimp::assimp yaml-cpp::yaml-cpp)
    set(PLATFORM_INCLUDES "")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3    REQUIRED IMPORTED_TARGET glfw3)
    pkg_check_modules(YAML_CPP REQUIRED IMPORTED_TARGET yaml-cpp)
    find_package(glm    REQUIRED)
    find_package(assimp REQUIRED)

    set(PLATFORM_LIBS PkgConfig::GLFW3 glm::glm assimp::assimp PkgConfig::YAML_CPP)
    set(PLATFORM_INCLUDES ${YAML_CPP_INCLUDE_DIRS})
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})

# Pass the source root to C++ so FileSystem can resolve asset paths at runtime
target_compile_definitions(${PROJECT_NAME} PRIVATE ROOT_DIR=\"${CMAKE_SOURCE_DIR}\")

target_include_directories(${PROJECT_NAME} PRIVATE
    "include"
    ${OPENGL_INCLUDE_DIR}
    ${PLATFORM_INCLUDES}
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    ${PLATFORM_LIBS}
    ${CMAKE_DL_LIBS}
)

# Copy assets to the build directory so the binary can find them at runtime
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/default_scene.yaml DESTINATION ${CMAKE_BINARY_DIR})
