# Physics Sandbox

## Project Info
Welcome to **Physics Sandbox**, a versatile C++20 project designed for physics simulations. This project can be used as:
- A standalone executable with a Unity-inspired UI for interactive sandbox exploration.
- A static or shared library for integration into games or other simulation software.

**Version:** 1.0.0  
**Inspired by:** Unity Editor/Player  
**Build System:** CMake  
**Target Hardware:** NVIDIA RTX 4060  

The goal is to provide a flexible platform for experimenting with physics bodies, forces, and vector fields, rendered with Vulkan and styled with NanoVG.

## Project Dependencies
To build and run Physics Sandbox, you need the following dependencies:
- **CMake** (version 3.20 or higher)
- **C++ Compiler** supporting C++20 (e.g., Visual Studio 2022, GCC, Clang)
- **SDL3** (version 3.0.0 or higher) with Vulkan support
- **Vulkan SDK** (version 1.3 or higher)
- **NanoVG** (included via Git submodule)
- **Assimp** (included via Git submodule) for FBX loading
- **Boost** (included via Git submodule) for Filesystem and Serialization
- **Google Test** (optional, included via Git submodule) for unit testing
- **spdlog** (optional, included via Git submodule) for logging
- **GLM** (included via Git submodule) for math operations
- **stb_image** (included via Git submodule) for texture loading

**Installation Instructions:**
- Clone the repository with submodules: `git clone --recurse-submodules https://github.com/yourusername/physics-sandbox.git`
- Install system dependencies via your package manager (e.g., `apt`, `brew`, or Windows installer for Vulkan SDK).

## Guide to Build
### Prerequisites
Ensure all dependencies are installed and the Vulkan SDK's `glslc` is in your PATH.

### Steps
1. **Set Up Directory Structure**
   - Windows: Run `setup_dirs.bat`
   - Linux/macOS: Run `chmod +x setup_dirs.sh && ./setup_dirs.sh`

2. **Build Options**
   - Use one of the following scripts based on your needs:
     - **Static Library:** 
       - Windows: `create_static_lib.bat`
       - Linux/macOS: `chmod +x create_static_lib.sh && ./create_static_lib.sh`
     - **Shared Library:** 
       - Windows: `create_shared_lib.bat`
       - Linux/macOS: `chmod +x create_shared_lib.sh && ./create_shared_lib.sh`
     - **Executable:** 
       - Windows: `create_executable.bat`
       - Linux/macOS: `chmod +x create_executable.sh && ./create_executable.sh`

3. **Clean Build (Optional)**
   - Windows: `clean.bat`
   - Linux/macOS: `chmod +x clean.sh && ./clean.sh`

4. **Run Tests (Optional, for Executable Mode)**
   - Windows: `run_tests.bat`
   - Linux/macOS: `chmod +x run_tests.sh && ./run_tests.sh`

## Installation
- **Executable:** After building with `create_executable`, copy the `build/bin/sandbox` executable to your desired location.
- **Library:** After building with `create_static_lib` or `create_shared_lib`, link `build/lib/sandbox.lib` (Windows) or `build/lib/libsandbox.so` (Linux/macOS) in your project. Include `inc/` headers in your include path.

## Usage
- **Sandbox Executable:** Run the `sandbox` executable to launch the interactive physics sandbox. Use the UI to add bodies, apply forces, and switch between Design/Play modes.
- **Library:** Include the library in your project and use the API (e.g., `PhysicsBody`, `Force`) to simulate physics. See `inc/` headers for details (API docs forthcoming).

## Contributing
We welcome contributions! Please:
- Fork the repository and create a feature branch.
- Follow C++20 best practices.
- Submit pull requests with clear descriptions.
- Set up your environment by following the Guide to Build.

## License
This project is licensed under the [MIT License](LICENSE). See the `LICENSE` file for details.

## Documentation
- **Design Documents:** Located in `docs/design/`.
- **API Documentation:** Generated with Doxygen in `docs/api/` (run `cmake --build . --target docs` after building).
- **Further Reading:** Check `docs/` for additional resources.

## Known Issues and Limitations
- Currently lacks advanced physics features (e.g., fluid dynamics).
- Shader compilation requires `glslc` in PATH—ensure Vulkan SDK is installed.
- macOS support is experimental; report issues for improvement.

## Roadmap
- Add support for fluid and soft body simulations.
- Enhance UI with more gizmo options and presets.
- Improve cross-platform compatibility (e.g., Android, iOS).
