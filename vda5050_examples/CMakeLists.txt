cmake_minimum_required(VERSION 3.20)
project(vda5050_examples)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(vda5050_bt_execution REQUIRED)
find_package(vda5050_core REQUIRED)

include(GNUInstallDirs)

add_executable(bt_execution_main
  src/bt_execution_main.cpp
  src/vda5050_examples/robot_adapter.cpp
)
target_link_libraries(bt_execution_main
  PUBLIC
    vda5050_bt_execution::vda5050_bt_execution
)
target_include_directories(bt_execution_main
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
  TARGETS bt_execution_main
  EXPORT bt_execution_main
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

add_executable(master_control src/master_control.cpp)
target_link_libraries(master_control
  PUBLIC
    vda5050_core::vda5050_core
    vda5050_types::vda5050_types
    vda5050_json_utils::vda5050_json_utils
    vda5050_bt_execution::vda5050_bt_execution
)

install(
  TARGETS master_control
  EXPORT master_control
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY launch config worlds
  DESTINATION share/${PROJECT_NAME}
)

ament_export_dependencies(bt_execution_main
  vda5050_core
  vda5050_bt_execution
  behaviortree_cpp
  # to be removed later
  rclcpp
  geometry_msgs
  tf2_geometry_msgs
  # to be removed later
)

if(BUILD_TESTING)
  find_package(ament_cmake_cppcheck REQUIRED)
  ament_cppcheck()

  find_package(ament_cmake_cpplint REQUIRED)
  ament_cpplint()

  find_package(ament_cmake_clang_format REQUIRED)
  ament_clang_format(
    CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../.clang-format"
  )

  find_package(ament_cmake_copyright REQUIRED)
  ament_copyright()
endif()

ament_package()
