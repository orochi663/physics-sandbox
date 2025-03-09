cmake_minimum_required(VERSION 3.20)

# Project name and version
project(sandbox VERSION 1.0.0)

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

# C++ standard check (require C++20, fallback to C++17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX20)
    message(STATUS "Compiler supports C++20")
elseif(COMPILER_SUPPORTS_CXX17)
    message(WARNING "Compiler does not support C++20, falling back to C++17")
    set(CMAKE_CXX_STANDARD 17)
else()
    message(FATAL_ERROR "Compiler does not support C++17 or C++20")
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

# Add sanitizers for Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug AND ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Find dependencies with version checks
find_package(SDL3 3.0.0 REQUIRED)
if(NOT SDL3_VULKAN)
    message(FATAL_ERROR "SDL3 does not support Vulkan")
endif()
message(STATUS "Found SDL3 with Vulkan support")

find_package(Vulkan 1.3 REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()
message(STATUS "Found Vulkan")

# Shader compilation
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
add_custom_target(shaders ALL DEPENDS ${SHADER_OUTPUTS})

# Build third-party libraries
add_subdirectory(thirdparty/nanovg)
add_subdirectory(thirdparty/assimp)
add_subdirectory(thirdparty/boost)
add_subdirectory(thirdparty/gtest)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/stb_image)

# Include directories
include_directories(
    inc
    inc/ui
    inc/graphics
    inc/physics
    inc/utils
    thirdparty/SDL3/include
    thirdparty/Vulkan/include
    thirdparty/nanovg/src
    thirdparty/assimp/include
    thirdparty/boost
    thirdparty/gtest/include
    thirdparty/spdlog/include
    thirdparty/glm
    thirdparty/stb_image
)

# Source directories
add_subdirectory(src/ui)
add_subdirectory(src/graphics)
add_subdirectory(src/physics)
add_subdirectory(src/utils)

# Collect all source files
file(GLOB_RECURSE UI_SOURCES src/ui/*.cpp)
file(GLOB_RECURSE GRAPHICS_SOURCES src/graphics/*.cpp)
file(GLOB_RECURSE PHYSICS_SOURCES src/physics/*.cpp)
file(GLOB_RECURSE UTILS_SOURCES src/utils/*.cpp)

# Precompiled headers
target_precompile_headers(sandbox PRIVATE inc/pch.hpp)

# Library or Executable based on BUILD_MODE
if(BUILD_MODE MATCHES "LIBRARY")
    # Option for static or shared library
    option(BUILD_SHARED_LIBS "Build shared library" OFF)
    add_library(sandbox_lib ${UI_SOURCES} ${GRAPHICS_SOURCES} ${PHYSICS_SOURCES} ${UTILS_SOURCES})
    set_target_properties(sandbox_lib PROPERTIES OUTPUT_NAME "sandbox")
    target_link_libraries(sandbox_lib
        SDL3::SDL3
        Vulkan::Vulkan
        nanovg
        assimp
        Boost::filesystem
        Boost::serialization
        spdlog
        glm
        stb_image
    )
    if(BUILD_SHARED_LIBS)
        set_target_properties(sandbox_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
        message(STATUS "Building shared library: sandbox_lib")
    else()
        message(STATUS "Building static library: sandbox_lib")
    endif()
elseif(BUILD_MODE MATCHES "EXECUTABLE")
    # Create executable
    add_executable(sandbox
        main.cpp
        ${UI_SOURCES}
        ${GRAPHICS_SOURCES}
        ${PHYSICS_SOURCES}
        ${UTILS_SOURCES}
    )
    # Make sandbox depend on shaders
    add_dependencies(sandbox shaders)
    # Link libraries
    target_link_libraries(sandbox
        SDL3::SDL3
        Vulkan::Vulkan
        nanovg
        assimp
        Boost::filesystem
        Boost::serialization
        spdlog
        glm
        stb_image
    )
    if(TARGET sandbox_lib)
        target_link_libraries(sandbox sandbox_lib)
    endif()
endif()

# Platform-specific configurations
if(WIN32)
    add_compile_definitions(_WIN32)
elseif(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    target_link_libraries(sandbox ${COREFOUNDATION_LIBRARY})
elseif(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(sandbox X11)
endif()

# Copy assets to build directory
file(GLOB ASSET_DIRS "assets/*")
if(NOT ASSET_DIRS)
    message(WARNING "No assets found in assets/ - ensure assets are present")
endif()
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Enable testing
if(BUILD_TESTS AND BUILD_MODE MATCHES "EXECUTABLE")
    enable_testing()
    add_subdirectory(tests)
endif()

# Install target
if(BUILD_MODE MATCHES "EXECUTABLE")
    install(TARGETS sandbox DESTINATION bin)
    install(DIRECTORY assets DESTINATION bin)
elseif(BUILD_MODE MATCHES "LIBRARY")
    install(TARGETS sandbox_lib DESTINATION lib)
    install(DIRECTORY inc DESTINATION include)
endif()

# Doxygen for documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

# Static analysis
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(static_analysis
        COMMAND ${CPPCHECK} --enable=all --std=c++20 --suppress=missingIncludeSystem ${CMAKE_SOURCE_DIR}/src
        COMMENT "Running static analysis with cppcheck"
    )
endif()

find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
endif()

# Code coverage
if(CMAKE_BUILD_TYPE MATCHES Debug AND CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND BUILD_TESTS AND BUILD_MODE MATCHES "EXECUTABLE")
    add_compile_options(--coverage)
    add_link_options(--coverage)
    add_custom_target(coverage
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --html --output coverage.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report"
    )
endif()

# CPack for packaging
include(CPack)
set(CPACK_PACKAGE_NAME "PhysicsSandbox")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A physics sandbox application or library")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "ZIP;TGZ")
if(BUILD_MODE MATCHES "EXECUTABLE")
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};sandbox;ALL;/")
elseif(BUILD_MODE MATCHES "LIBRARY")
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};sandbox_lib;ALL;/")
endif()