cmake_minimum_required(VERSION 3.12)
project(icast)

# Define the Python script
set(PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/icast_idl.py)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Running icast_idl..."
    COMMAND python ${PYTHON_SCRIPT} dictionary.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE PYTHON_SCRIPT_RESULT
)

# Check the result of the Python script execution
if(NOT PYTHON_SCRIPT_RESULT EQUAL 0)
    message(FATAL_ERROR "Error running Python script: ${PYTHON_SCRIPT_RESULT}")
endif()

if(NOT DEFINED ENV{ICAST_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo "Saving icast path..."
        COMMAND bash -c "echo 'export ICAST_DIR=${CMAKE_CURRENT_SOURCE_DIR}' >> /home/$ENV{USER}/.bashrc"
    )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)

# Add source files
set(EXAMPLE_SOURCES
    example/simple.cpp
)

set(LIB_SOURCES
    src/dictionary.cpp
    src/multicast3.cpp
    src/icast.cpp
 )

add_library(icast ${LIB_SOURCES})

# Add executable
add_executable(simple example/simple.cpp)
target_link_libraries(simple icast)

set(CMAKE_INSTALL_INCLUDEDIR /usr/local/include)
set(CMAKE_INSTALL_LIBDIR /usr/local/lib)

target_include_directories(icast PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utils>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install rapidjson
install(DIRECTORY utils/rapidjson DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install library
install(TARGETS icast
  EXPORT ${PROJECT_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(icast_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}-targets
  DESTINATION ${icast_INSTALL_CONFIGDIR}
)
