cmake_minimum_required(VERSION 3.28) # Native C++20 Module support features
project(Hernan_Engine)

# Add option to enable/disable C++ 20 module
option(ENABLE_CPP20_MODULE "Enable C++ 20 module support for Vulkan" OFF)
# Enable C++ module dependency scanning only if C++ 20 module is enabled
if(ENABLE_CPP20_MODULE)
  set(CMAKE_CXX_SCAN_FOR_MODULES ON)
endif()

# Force vcpkg to allow looking for the host system's Vulkan SDK installation
set(VCPKG_FORCE_SYSTEM_BINARIES TRUE)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find libraries installed on the system
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

# Use pkg_check_modules for yaml-cpp for security in Ubuntu
find_package(PkgConfig REQUIRED)
if(WIN32)
	find_package(yaml-cpp CONFIG REQUIRED)
	set(YAML_LIB yaml-cpp::yaml-cpp)
else()
	pkg_check_modules(YAML_CPP REQUIRED yaml-cpp) # robust way to find yaml-cpp on Linux
	set(YAML_LIB yaml-cpp)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
# Backend-specific sources are compiled into their own libraries below,
# so exclude them from EngineCore to avoid duplicate-symbol link errors.
list(FILTER SOURCES EXCLUDE REGEX "/src/(opengl|vulkan)/")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/VKTemplate/cmake")

# ============== Core ================
# set(CORE_SOURCES)

# IMGUI 
# IMGUI source files
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui")
set(CORE_IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp    
)


add_library(EngineCore STATIC ${SOURCES} ${CORE_IMGUI_SOURCES})

# Absolute path to the project root, so asset/scene paths don't depend on the cwd
target_compile_definitions(EngineCore PUBLIC ROOT_DIR="${CMAKE_SOURCE_DIR}/")

target_include_directories(EngineCore PUBLIC
	"include"
	${IMGUI_DIR}
  ${IMGUI_DIR}/backends
	${YAML_CPP_INCLUDE_DIRS}
)

target_link_libraries(EngineCore PUBLIC
	glfw
	glm::glm
	assimp::assimp
	${YAML_LIB}
	${CMAKE_DL_LIBS} # Needed for some loaders like GLAD
)


# Copy Assets (So scene.yaml works)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/default_scene.yaml DESTINATION ${CMAKE_BINARY_DIR})

# ============== OpenGL ================
set(GL_SOURCES
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  src/opengl/glEditor.cpp
  src/opengl/glEngine.cpp
)

add_library(EngineBackend_GL STATIC 
  ${GL_SOURCES}
  
)

# Target needs to know about ImGui backends folders
target_include_directories(EngineBackend_GL PRIVATE 
	${OPENGL_INCLUDE_DIR}
	${IMGUI_DIR}/backends
)

# Link core engine and the OpenGL system libraries
target_link_libraries(EngineBackend_GL PUBLIC 
	EngineCore
	OpenGL::GL
)

# ============== Vulkan ================

file(TO_CMAKE_PATH "$ENV{VULKAN_SDK}" VULKAN_SDK_PATH)
find_program(SLANGC_EXECUTABLE slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)
find_package(Vulkan REQUIRED MODULE)
find_package(tinyobjloader REQUIRED)
find_package(tinygltf REQUIRED)
find_package(KTX REQUIRED)
find_package(stb REQUIRED)
set(STB_INCLUDEDIR ${stb_INCLUDE_DIRS})


# Set up Vulkan C++ module only if enabled
if(ENABLE_CPP20_MODULE)
  add_library(VulkanCppModule)
  add_library(Vulkan::cppm ALIAS VulkanCppModule)

  target_compile_definitions(VulkanCppModule PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1)
  target_include_directories(VulkanCppModule PUBLIC "${Vulkan_INCLUDE_DIR}")
  target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)

  set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

  # Add MSVC-specific compiler options for proper C++ module support
  if(MSVC)
    target_compile_options(VulkanCppModule PRIVATE
      /std:c++latest      # Use latest C++ standard for better module support
      /permissive-        # Standards conformance mode
      /Zc:__cplusplus     # Enable correct __cplusplus macro
      /EHsc               # Enable C++ exception handling
      /Zc:preprocessor    # Use conforming preprocessor
      /translateInclude   # Automatically translate #include to import for standard library
    )
  endif()

  target_sources(VulkanCppModule
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS
    "${Vulkan_INCLUDE_DIR}"
    FILES
    "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
  )


  # Add the vulkan.cppm file directly as a source file
  target_sources(VulkanCppModule
    PRIVATE
    "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
  )
else()
  # Create a dummy interface library when C++ 20 module is disabled
  add_library(VulkanCppModule INTERFACE)
  add_library(Vulkan::cppm ALIAS VulkanCppModule)
  target_link_libraries(VulkanCppModule INTERFACE Vulkan::Vulkan)
  target_compile_definitions(VulkanCppModule
    INTERFACE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
  )
endif()





set(VULKAN_SOURCES
  src/vulkan/vkEditor.cpp
  src/vulkan/vkEngine.cpp
	${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

add_library(EngineBackend_Vulkan STATIC ${VULKAN_SOURCES})

# Link the core engine and the Vulkan SDK targets
target_link_libraries(EngineBackend_Vulkan PUBLIC 
	EngineCore
	Vulkan::cppm
)


# ============== MAIN ================

# Main entry point
add_executable(${PROJECT_NAME} src/main.cpp)
# Root macro for asset paths
target_compile_definitions(${PROJECT_NAME} PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}/")
# Link core engine and backends to activate
target_link_libraries(${PROJECT_NAME} PRIVATE
	EngineCore
	#EngineBackend_GL
	EngineBackend_Vulkan
)

# Add compile definition if C++ 20 module is enabled
if(ENABLE_CPP20_MODULE)
  target_compile_definitions(${PROJECT_NAME} PRIVATE USE_CPP20_MODULES=1)
endif()

# Define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS to treat VK_ERROR_OUT_OF_DATE_KHR as a success code
target_compile_definitions(${PROJECT_NAME} PRIVATE "VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS" )

# Slang shader
set (SHADER_TARGET ${PROJECT_NAME}Shader)
set (SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders)
set (SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/VKTemplate/shader.slang")
set (ENTRY_POINTS -entry vertMain -entry fragMain)
add_custom_command(OUTPUT ${SHADERS_DIR} COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR})
add_custom_command(
  OUTPUT  ${SHADERS_DIR}/slang.spv
  COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o ${SHADERS_DIR}/slang.spv
  WORKING_DIRECTORY ${SHADERS_DIR}
  DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
  COMMENT "Compiling Slang Shaders"
  VERBATIM
)
add_custom_target(${SHADER_TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)
add_dependencies(${PROJECT_NAME} ${SHADER_TARGET})

if(WIN32)
  if(${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${SHADERS_DIR})
  endif()
endif()



