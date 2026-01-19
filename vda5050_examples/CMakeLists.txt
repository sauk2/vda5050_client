cmake_minimum_required(VERSION 3.20)
project(vda5050_examples)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(vda5050_core REQUIRED)
find_package(vda5050_types REQUIRED)
find_package(vda5050_json_utils REQUIRED)
find_package(vda5050_execution REQUIRED)

include(GNUInstallDirs)

add_executable(sequential_execution src/sequential_execution.cpp)

target_link_libraries(sequential_execution
  PUBLIC
    vda5050_core::vda5050_core
    vda5050_types::vda5050_types
    vda5050_json_utils::vda5050_json_utils
    vda5050_execution::vda5050_execution
)

target_include_directories(sequential_execution
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_executable(execution_handler src/execution_handler.cpp)

target_link_libraries(execution_handler
  PUBLIC
    vda5050_core::vda5050_core
    vda5050_types::vda5050_types
    vda5050_json_utils::vda5050_json_utils
    vda5050_execution::vda5050_execution
)

target_include_directories(execution_handler
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
  TARGETS sequential_execution
  EXPORT sequential_execution
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(
  TARGETS execution_handler
  EXPORT execution_handler
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  include(../cmake/vda5050_lint_common.cmake)
  vda5050_lint_common()
endif()

ament_package()
