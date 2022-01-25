# Define minimum version of CMake
cmake_minimum_required(VERSION 3.13.1)

# Set project name
project(app)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Location of executable source file. Target must be named "app."
add_executable(app
    source/main.cpp
    )

# Force use of C++ compiler (so main in C or C++ can call C++ functions)
set_source_files_properties(source/main.cpp PROPERTIES LANGUAGE CXX)

# Location of impulse library
add_subdirectory(lib)

# Remove this line on ARM Cortex A processors
target_compile_definitions(app PUBLIC TF_LITE_DISABLE_X86_NEON=1)