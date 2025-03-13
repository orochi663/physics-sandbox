cmake_minimum_required(VERSION 3.5)

# Correct project declaration with LANGUAGES keyword.
project(sandbox VERSION 1.0.0 LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Build Output Directories (all outputs go to dedicated subfolders in the build directory)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Custom build options
option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_SANITIZERS "Enable sanitizers in Debug builds" ON)
option(ENABLE_VERBOSE "Enable verbose CMake output" OFF)
set(BUILD_MODE "EXECUTABLE" CACHE STRING "Build mode: LIBRARY or EXECUTABLE")
set_property(CACHE BUILD_MODE PROPERTY STRINGS LIBRARY EXECUTABLE)

if(ENABLE_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Set C++ standard (strictly require C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adjust flags based on compiler
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/std:c++20)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-std=c++20)
endif()

# Check if C++20 is supported
include(CheckCXXCompilerFlag)
if(NOT CMAKE_CXX20_STANDARD_COMPILE_OPTION)
    message(FATAL_ERROR "C++20 not supported")
endif()

# Debug/Release build configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug/Release)" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Compile options for Debug/Release
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-g -O0)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_options(-O3)
endif()

# Add warning flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
endif()

# Add sanitizers for Debug builds (GNU/Clang only)
if(CMAKE_BUILD_TYPE MATCHES Debug AND ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# ------------------------------------------------------------------------------
# Find dependencies (assumed to be installed by the user)
find_package(SDL3 REQUIRED)
if(NOT SDL3_FOUND)
    message(FATAL_ERROR "SDL3 not found")
endif()
message(STATUS "Found SDL3")

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()
message(STATUS "Found Vulkan")

find_package(glm CONFIG REQUIRED)
if(NOT glm_FOUND)
    message(FATAL_ERROR "glm not found")
endif()
message(STATUS "Found glm")

find_package(spdlog CONFIG REQUIRED)
if(NOT spdlog_FOUND)
    message(FATAL_ERROR "spdlog not found")
endif()
message(STATUS "Found spdlog")

find_package(nanovg CONFIG REQUIRED)
if(NOT nanovg_FOUND)
    message(FATAL_ERROR "nanovg not found")
endif()
message(STATUS "Found nanovg")

find_package(nanovg CONFIG REQUIRED)
if(NOT nanovg_FOUND)
    message(FATAL_ERROR "nanovg not found")
endif()
message(STATUS "Found nanovg")

find_package(nlohmann_json CONFIG REQUIRED)
if(NOT nlohmann_json_FOUND)
    message(FATAL_ERROR "nlohmann json not found")
endif()
message(STATUS "Found json nlohmann")

# ------------------------------------------------------------------------------
# Include directories: add our 'inc' directory to include all headers
include_directories("${CMAKE_SOURCE_DIR}/inc")

# ------------------------------------------------------------------------------
# Shader compilation as a standalone target
find_program(GLSLC glslc)
if(NOT GLSLC)
    message(FATAL_ERROR "glslc not found - required for shader compilation")
endif()

file(GLOB SHADER_FILES "assets/shaders/*.vert" "assets/shaders/*.frag")
if(NOT SHADER_FILES)
    message(WARNING "No shaders found in assets/shaders/ - ensure shaders are present")
endif()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_OUTPUT "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv")
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${GLSLC} ${SHADER} -o ${SHADER_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${SHADER_NAME}"
    )
    list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
endforeach()
add_custom_target(shaders DEPENDS ${SHADER_OUTPUTS})

# ------------------------------------------------------------------------------
# Collect source files from subdirectories using aux_source_directory.
#
# Note: This approach automatically aggregates all .cpp files found in the directories.
# ------------------------------------------------------------------------------
aux_source_directory(${CMAKE_SOURCE_DIR}/src/ui SOURCES_UI)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/graphics SOURCES_GRAPHICS)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/physics SOURCES_PHYSICS)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/utils SOURCES_UTILS)

# Main source file (ensure main.cpp exists in your project root's src folder)
set(MAIN_SOURCE ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Aggregate all source files into one variable
set(SOURCES ${MAIN_SOURCE} ${SOURCES_UI} ${SOURCES_GRAPHICS} ${SOURCES_PHYSICS} ${SOURCES_UTILS})

# Glob header files from the 'inc' directory (recursively)
file(GLOB_RECURSE PROJECT_HEADERS
     "${CMAKE_SOURCE_DIR}/inc/*.h"
     "${CMAKE_SOURCE_DIR}/inc/*.hpp")

# ------------------------------------------------------------------------------
# Define target based on BUILD_MODE and add header files so that Visual Studio displays them.
# ------------------------------------------------------------------------------
if(BUILD_MODE MATCHES "LIBRARY")
    # Option for static or shared library
    option(BUILD_SHARED_LIBS "Build shared library" OFF)
    if(BUILD_SHARED_LIBS)
        add_library(sandbox_lib SHARED ${SOURCES})
    else()
        add_library(sandbox_lib STATIC ${SOURCES})
    endif()
    message(STATUS "Configured as a library target: sandbox_lib")
    
    # Add headers to the library target for Visual Studio
    target_sources(sandbox_lib PRIVATE ${PROJECT_HEADERS})
    source_group(TREE "${CMAKE_SOURCE_DIR}/inc" PREFIX "Header Files" FILES ${PROJECT_HEADERS})
else()  # BUILD_MODE is EXECUTABLE
    add_executable(${PROJECT_NAME} ${SOURCES})
    message(STATUS "Configured as an executable target")
    
    # Add headers to the executable target for Visual Studio
    target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_HEADERS})
    source_group(TREE "${CMAKE_SOURCE_DIR}/inc" PREFIX "Header Files" FILES ${PROJECT_HEADERS})
    
    target_link_libraries(sandbox
        SDL3::SDL3
        Vulkan::Vulkan
        nanovg::nanovg
        spdlog::spdlog
        glm::glm
		nlohmann_json::nlohmann_json
    )
endif()

# Platform-specific configurations
if(WIN32)
    add_compile_definitions(_WIN32)
elseif(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    if(BUILD_MODE MATCHES "EXECUTABLE")
        target_link_libraries(sandbox ${COREFOUNDATION_LIBRARY})
    endif()
elseif(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    if(BUILD_MODE MATCHES "EXECUTABLE")
        target_link_libraries(sandbox X11)
    endif()
endif()

# ------------------------------------------------------------------------------
# INSTALL target: Installs executable/library and headers.
# ------------------------------------------------------------------------------
if(BUILD_MODE MATCHES "EXECUTABLE")
    # Install the executable to the 'bin' directory of the install prefix.
    install(TARGETS sandbox
            RUNTIME DESTINATION bin)
else()  # LIBRARY mode: install the library and headers.
    install(TARGETS sandbox_lib
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)
    # Install all public headers from the 'inc' directory.
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/inc/ DESTINATION include)
endif()

# ------------------------------------------------------------------------------
# Code coverage (only for Debug builds with GNU and if tests are enabled in executable mode)
# ------------------------------------------------------------------------------
if(CMAKE_BUILD_TYPE MATCHES Debug AND CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND BUILD_TESTS AND BUILD_MODE MATCHES "EXECUTABLE")
    add_compile_options(--coverage)
    add_link_options(--coverage)
    add_custom_target(coverage
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --html --output coverage.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report"
    )
endif()

# ------------------------------------------------------------------------------
# CPack for packaging
# ------------------------------------------------------------------------------
include(CPack)
set(CPACK_PACKAGE_NAME "PhysicsSandbox")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A physics sandbox application or library")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "ZIP;TGZ")
if(BUILD_MODE MATCHES "EXECUTABLE")
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};sandbox;ALL;/")
else()  # LIBRARY mode
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};sandbox_lib;ALL;/")
endif()

