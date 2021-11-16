cmake_minimum_required (VERSION 3.5)
project(crazyflie-lib-cpp)

#disable pybind11
option(BUILD_PYTHON_BINDINGS OFF)

# crazyflie-link-cpp dependencies
add_subdirectory(crazyflie-link-cpp)

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
# C++ library
target_include_directories(crazyflieLibCpp PUBLIC
  include
)
target_link_libraries(crazyflieLibCpp PUBLIC crazyflieLinkCpp)



# example_basicLog
add_executable(example_basicLog
  examples/basicLog.cpp
)
target_link_libraries(example_basicLog
  crazyflieLibCpp
)

# example_basicToc
add_executable(example_basicToc
  examples/basicToc.cpp
)
target_link_libraries(example_basicToc
  crazyflieLibCpp
)

# example_appChannelTest
add_executable(example_appChannelTest
  examples/appChannelTest.cpp
)
target_link_libraries(example_appChannelTest
  crazyflieLibCpp
)

# example_logViaAppChannel
add_executable(example_logViaAppChannel
  examples/logViaAppChannel.cpp
)
target_link_libraries(example_logViaAppChannel
  crazyflieLibCpp
)

# example_connectionWorkerTest
add_executable(example_connectionWorkerTest
  examples/connectionWorkerTest.cpp
)
target_link_libraries(example_connectionWorkerTest
  crazyflieLibCpp
)

# example_resetUsb
add_executable(example_resetUsb
  examples/resetUsb.cpp
)
target_link_libraries(example_resetUsb
  crazyflieLibCpp
)

# example_logConnectionWorker
add_executable(example_logConnectionWorker
  examples/logConnectionWorker.cpp
)
target_link_libraries(example_logConnectionWorker
  crazyflieLibCpp
)


# example_crazyflieLog
add_executable(example_crazyflieLog
  examples/crazyflieLog.cpp
)
target_link_libraries(example_crazyflieLog
  crazyflieLibCpp
)

# example_stressTest
add_executable(example_stressTest
  examples/stressTest.cpp
)
target_link_libraries(example_stressTest
  crazyflieLibCpp
)


