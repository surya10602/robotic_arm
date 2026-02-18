cmake_minimum_required(VERSION 3.14)
project(RoboticArmIK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/libs)


set(ONNX_ROOT ${CMAKE_SOURCE_DIR}/libs/onnxruntime)
include_directories(${ONNX_ROOT}/include)

find_library(ONNX_LIB 
    NAMES onnxruntime 
    PATHS ${ONNX_ROOT}/lib
    NO_DEFAULT_PATH
)

if(NOT ONNX_LIB)
    message(FATAL_ERROR "ONNX Runtime library not found! Check libs/onnxruntime/lib folder.")
endif()

add_executable(robot_control 
    src/main.cpp 
    src/RobotArm.cpp 
    src/TrajectoryPlanner.cpp 
    src/MLSolver.cpp
)

target_link_libraries(robot_control ${ONNX_LIB})
target_include_directories(robot_control PRIVATE include)

add_custom_command(TARGET robot_control POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ONNX_ROOT}/lib/onnxruntime.dll"
    $<TARGET_FILE_DIR:robot_control>
)