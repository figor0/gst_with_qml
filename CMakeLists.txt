cmake_minimum_required(VERSION 3.5)

project(gst_qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package( Qt5GStreamer REQUIRED )
#find_package( GLIB2 )

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

# Tell CMake to create the helloworld executable
add_executable(${PROJECT_NAME} main.cpp)

# Use the Core module from Qt5
target_link_libraries(${PROJECT_NAME} ${QTGSTREAMER_UTILS_LIBRARIES})
