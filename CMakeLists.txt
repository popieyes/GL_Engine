cmake_minimum_required(VERSION 3.10)
project(GL_Engine)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# IMGUI 
# IMGUI source files
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)


# Find libraries installed on the system
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
# Use pkg_check_modules for yaml-cpp for security in Ubuntu
find_package(PkgConfig REQUIRED)
if(WIN32)
	find_package(yaml-cpp CONFIG REQUIRED)
else()
	pkg_check_modules(YAML_CPP REQUIRED yaml-cpp) # robust way to find yaml-cpp on Linux
endif()
# Recursive search in src of .cpp files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

# Pass the folder path to C++ as a string macro
target_compile_definitions(${PROJECT_NAME} PRIVATE ROOT_DIR=\"${CMAKE_SOURCE_DIR}\")
# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
	"include" # local header folder
	${OPENGL_INCLUDE_DIR}
	${YAML_CPP_INCLUDE_DIRS}
	${IMGUI_DIR}
	${IMGUI_DIR}/backends
)

# Link Libraries
target_link_libraries(${PROJECT_NAME}
	OpenGL::GL
	glfw
	glm::glm
	assimp::assimp
	yaml-cpp::yaml-cpp
	${CMAKE_DL_LIBS} # Needed for some loaders like GLAD

)

# Copy Assets (So scene.yaml works)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/default_scene.yaml DESTINATION ${CMAKE_BINARY_DIR})
