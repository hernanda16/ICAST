cmake_minimum_required(VERSION 3.12)
project(icast)

# Set C++ standard
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(EXAMPLE_SOURCES
    example/simple.cpp
)

set(LIB_SOURCES
    src/dictionary.cpp
    src/multicast.cpp
    src/multicast2.cpp
    src/icast.cpp
 )

# Add executable
# add_executable(simple ${EXAMPLE_SOURCES} ${LIB_SOURCES})
add_executable(simple2 example/simple2.cpp ${LIB_SOURCES})