# Non-Atomic Action Interpreter Module

This module is part of the ostis-ps-lib and is responsible for interpreting non-atomic actions within the OSTIS.

## Using module as an Extension for sc-machine

The non-atomic action interpreter Module can be used as an extension to the sc-machine. Follow these steps to integrate it:

### 1. Download sc-machine Artifacts

   - Download pre-built artifacts of sc-machine from [GitHub Releases](https://github.com/ostis-ai/sc-machine/releases) and extract them to a location of your choice.
   - Alternatively, you can build sc-machine from source and use the resulting artifacts.
   - For more details on installing and using sc-machine artifacts, refer to the [sc-machine quick start guide](https://ostis-ai.github.io/sc-machine/quick_start/).

### 2. Specify Extension Paths

   - When running the sc-machine binary, specify the path to the `lib/extensions` directory from the extracted Non-Atomic Action Interpreter Module folder:

   ```sh
   ./path/to/sc-machine/binary -s path/to/kb.bin \
       -e "non-atomic-action-interpreter-module/build/<Debug|Release>/lib/extensions;path/to/sc-machine/lib/extensions"
   ```

   Replace `path/to/sc-machine/binary` with the path to the sc-machine binary, and `path/to/kb.bin` with the path to your knowledge base file.

## Developing Module

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

First, clone the repository containing the Non-Atomic Action Interpreter Module:

```bash
git clone https://github.com/ostis-ai/ostis-ps-lib.git
cd ostis-ps-lib/non-atomic-action-interpreter-module
```

#### 2. Install Dependencies with Conan

The project uses Conan to manage dependencies. Install the required dependencies:

```bash
conan install . --build=missing
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
