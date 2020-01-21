project(hhtp CXX)

cmake_minimum_required (VERSION 3.5)

# -g allows for gdb debugging
# turn on -03 for best performance
add_definitions(-std=c++11)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${CXX_FLAGS}")

add_library(processparser SHARED src/ProcessParser.cc)
target_include_directories(processparser
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include)


add_library(process SHARED src/Process.cc)
target_link_libraries(process processparser)
target_include_directories(process
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include)


add_library(processcontainer SHARED src/ProcessContainer.cc)
target_link_libraries(processcontainer process)
target_include_directories(processcontainer
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include)


add_library(sysinfo SHARED src/SysInfo.cc)
target_link_libraries(sysinfo processparser)
target_include_directories(sysinfo
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include)


add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME}
	process processcontainer processparser sysinfo ncurses)
target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include)