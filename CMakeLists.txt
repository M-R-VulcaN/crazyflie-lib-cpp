cmake_minimum_required (VERSION 3.5)
project(crazyflie-lib-cpp)


# Enable C++14 and warnings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# dependencies
include(crazyflie-link-cpp/CMakeLists.txt)

# C++ library

add_library(crazyflieLibCpp
  src/ConnectionWrapper.cpp
  src/LoggingCrazyflieWrapper.cpp
  src/ConnectionWorker.cpp
  src/Crazyflie.cpp
  src/Toc.cpp
  src/TocWrapper.cpp
  src/debug.cpp
  src/Log.cpp
)

# target_include_directories(libcrazyflieLibCpp
#   PUBLIC
#     $<INSTALL_INTERFACE:include/crazyflie_cpp>
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/crazyflie_cpp>
# )



# Link pthread on Linux and Mac only
if (NOT MSVC)
target_link_libraries(crazyflieLibCpp
  PRIVATE
    pthread
)
endif()

target_link_libraries(crazyflieLibCpp
  PRIVATE
    ${LIBUSB_LIBRARY}
)
set_property(TARGET crazyflieLibCpp PROPERTY POSITION_INDEPENDENT_CODE ON)

# C++ example application
if (BUILD_CPP_EXAMPLES)
  # example_console
  add_executable(example_console
    examples/console.cpp
  )
  target_link_libraries(example_console
    crazyflieLinkCpp
  )

  # example_scan
  add_executable(example_scan
    examples/scan.cpp
  )
  target_link_libraries(example_scan
    crazyflieLinkCpp
  )

  # example_benchmark
  add_executable(example_benchmark
    examples/benchmark.cpp
  )
  target_link_libraries(example_benchmark
    crazyflieLinkCpp
  )

  # example_broadcast
  add_executable(example_broadcast
    examples/broadcast.cpp
  )
  target_link_libraries(example_broadcast
    crazyflieLinkCpp
  )
endif()

if (BUILD_PYTHON_BINDINGS)
  # Python bindings
  add_subdirectory(pybind11)

  pybind11_add_module(cflinkcpp
    src/python_bindings.cpp
  )

  target_link_libraries(cflinkcpp
    PRIVATE
      crazyflieLinkCpp
  )
endif()