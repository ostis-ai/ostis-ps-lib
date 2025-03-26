# Library of Common Utilities

It's a library that provides common utilities and functions, likely to be used across various components of ostis-systems.

## Using Library as a Conan Package

The Common library can be used as a Conan package in other projects. Here's how to integrate it:

### 1. Add the following to your `conanfile.txt`:

```txt
[requires]
ps-common-lib/0.1.0
```

### 2. Run Conan to install the package:

```bash
conan install . --build=missing
```

### 3. In your CMakeLists.txt, add the following to find and link the library:

```cmake
find_package(ps-common-lib REQUIRED)

add_executable(your_target your_source_files.cpp)
target_link_libraries(your_target PRIVATE ps-common-lib::common-utils)
```

### 4. Configure your project:

```sh
cmake --preset conan-release
```

### 5. Build your project as usual:

```sh
cmake --build --preset conan-release
```

### 6. Download sc-machine Artifacts

- Download pre-built artifacts of sc-machine from [GitHub Releases](https://github.com/ostis-ai/sc-machine/releases) and extract them to a location of your choice. After that you can use sc-machine binaries to load extensions with agents that use the Common library.

!!! Note
    The `ps-common-lib::common-utils` target provides all the necessary include directories and linked libraries. You don't need to specify them separately.

If you're using Conan's CMake integration in your project, you might also need to add the following to your `conanfile.py`:

```python
def generate(self):
    tc = CMakeToolchain(self)
    tc.generate()
    deps = CMakeDeps(self)
    deps.generate()
```

This ensures that the necessary CMake files are generated for finding and linking the Common library.

## Using Library in Your Code

After you've set up your project to use the Common Utilities library, you can start using its functionality in your code. Here's how to include and use the library:

1. Include the necessary headers in your C++ files:

```cpp
#include <ps-common-lib/utils/logic_utils.hpp>
#include <ps-common-lib/utils/relation_utils.hpp>
#include <ps-common-lib/utils/template_params_utils.hpp>
```

2. Refer to the library's header files for a complete list of available functions and their usage.

## Developing Library

### Installation Prerequisites

Before you begin, ensure you have the following installed:

- CMake (version 3.24 or higher);
- C++ Compiler with C++17 support;
- Conan package manager;
- ccache (for faster rebuilds).

!!! Note
    This project is not supported on Windows OS.

### Installation & Build

#### 1. Clone Repository

First, clone the repository containing the Common component:

```bash
git clone https://github.com/ostis-ai/ostis-ps-lib.git
cd ostis-ps-lib/ps-common-lib
```

#### 2. Install Dependencies with Conan

The project uses Conan to manage dependencies. Choose build mode and install the required dependencies:

- Debug:

```bash
conan install . -s build_type=Debug --build=missing
```

- Release

```bash
conan install . -s build_type=Release --build=missing
```

#### 3. Configure Project

You can configure the project using CMake presets. There are three main configuration options:

- Debug with tests:
  
  ```sh
  cmake --preset debug-conan
  ```

- Release:
  
  ```sh
  cmake --preset release-conan
  ```

- Release with tests:
  
  ```sh
  cmake --preset release-with-tests-conan
  ```

#### 4. Build Project

After configuring, you can build the project:

For debug build:

```sh
cmake --build --preset debug
```

For release build:

```sh
cmake --build --preset release
```

#### 5. Run Tests

If you've configured the project with tests, you can run them using CTest:

```sh
cd build/<Debug|Release>
ctest -V
```

### Configuration Options

The following options can be set when configuring the project:

- `SC_BUILD_TESTS`: Set to ON to build unit tests (default is OFF).
- `SC_CLANG_FORMAT_CODE`: Set to ON to add clang-format targets (default is OFF).
- `AUTO_CCACHE`: Set to ON to use ccache for faster rebuilds (default is ON).

Example of setting an option:

```sh
cmake --preset release-with-tests-conan -DSC_CLANG_FORMAT_CODE=ON <other_options>
```

## Troubleshooting

For any issues or questions about using this package, please refer to the project's GitHub repository or contact the OSTIS-AI team.
